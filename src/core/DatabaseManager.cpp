#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QCryptographicHash>
#include <QDebug>
#include <QVariant>

DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager inst;
    return inst;
}

bool DatabaseManager::open(const QString& path) {
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(path);
    m_open = m_db.open();
    if (!m_open) {
        qWarning() << "DB open failed:" << m_db.lastError().text();
        return false;
    }
    QSqlQuery pragma(m_db);
    pragma.exec("PRAGMA foreign_keys = ON");
    initSchema();
    seedIfEmpty();
    return true;
}

QSqlDatabase& DatabaseManager::db() { return m_db; }
bool DatabaseManager::isOpen() const { return m_open; }

void DatabaseManager::initSchema() {
    QSqlQuery q(m_db);
    const QStringList statements = {
        R"(CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT UNIQUE NOT NULL,
            password_hash TEXT NOT NULL,
            role TEXT NOT NULL CHECK(role IN ('Admin','Faculty','Student','Registrar')),
            linked_ref_id INTEGER,
            active INTEGER NOT NULL DEFAULT 1,
            created_at TEXT DEFAULT CURRENT_TIMESTAMP
        ))",
        R"(CREATE TABLE IF NOT EXISTS students (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            roll_no TEXT UNIQUE NOT NULL,
            first_name TEXT NOT NULL,
            last_name TEXT NOT NULL,
            email TEXT,
            phone TEXT,
            department TEXT,
            batch_year INTEGER,
            admission_date TEXT,
            status TEXT NOT NULL DEFAULT 'Active'
        ))",
        R"(CREATE TABLE IF NOT EXISTS faculty (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            employee_no TEXT UNIQUE NOT NULL,
            first_name TEXT NOT NULL,
            last_name TEXT NOT NULL,
            email TEXT,
            phone TEXT,
            department TEXT,
            designation TEXT,
            max_load_hours INTEGER DEFAULT 18
        ))",
        R"(CREATE TABLE IF NOT EXISTS courses (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            code TEXT UNIQUE NOT NULL,
            title TEXT NOT NULL,
            credit_hours INTEGER NOT NULL,
            department TEXT,
            semester TEXT,
            faculty_id INTEGER,
            seat_limit INTEGER NOT NULL DEFAULT 40,
            FOREIGN KEY(faculty_id) REFERENCES faculty(id) ON DELETE SET NULL
        ))",
        R"(CREATE TABLE IF NOT EXISTS prerequisites (
            course_id INTEGER NOT NULL,
            prerequisite_course_id INTEGER NOT NULL,
            PRIMARY KEY(course_id, prerequisite_course_id),
            FOREIGN KEY(course_id) REFERENCES courses(id) ON DELETE CASCADE,
            FOREIGN KEY(prerequisite_course_id) REFERENCES courses(id) ON DELETE CASCADE
        ))",
        R"(CREATE TABLE IF NOT EXISTS enrollments (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            student_id INTEGER NOT NULL,
            course_id INTEGER NOT NULL,
            semester TEXT NOT NULL,
            status TEXT NOT NULL DEFAULT 'Registered' CHECK(status IN ('Registered','Dropped','Completed')),
            enrolled_at TEXT DEFAULT CURRENT_TIMESTAMP,
            UNIQUE(student_id, course_id, semester),
            FOREIGN KEY(student_id) REFERENCES students(id) ON DELETE CASCADE,
            FOREIGN KEY(course_id) REFERENCES courses(id) ON DELETE CASCADE
        ))",
        R"(CREATE TABLE IF NOT EXISTS attendance (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            enrollment_id INTEGER NOT NULL,
            class_date TEXT NOT NULL,
            present INTEGER NOT NULL DEFAULT 1,
            UNIQUE(enrollment_id, class_date),
            FOREIGN KEY(enrollment_id) REFERENCES enrollments(id) ON DELETE CASCADE
        ))",
        R"(CREATE TABLE IF NOT EXISTS grades (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            enrollment_id INTEGER NOT NULL UNIQUE,
            marks REAL,
            grade_letter TEXT,
            grade_point REAL,
            FOREIGN KEY(enrollment_id) REFERENCES enrollments(id) ON DELETE CASCADE
        ))",
        R"(CREATE TABLE IF NOT EXISTS fee_structures (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            department TEXT,
            semester TEXT,
            amount REAL NOT NULL,
            description TEXT
        ))",
        R"(CREATE TABLE IF NOT EXISTS payments (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            student_id INTEGER NOT NULL,
            fee_structure_id INTEGER,
            amount_paid REAL NOT NULL,
            paid_at TEXT DEFAULT CURRENT_TIMESTAMP,
            receipt_no TEXT UNIQUE,
            FOREIGN KEY(student_id) REFERENCES students(id) ON DELETE CASCADE,
            FOREIGN KEY(fee_structure_id) REFERENCES fee_structures(id) ON DELETE SET NULL
        ))"
    };
    for (const auto& s : statements) {
        if (!q.exec(s)) qWarning() << "Schema error:" << q.lastError().text() << s.left(40);
    }
}

void DatabaseManager::seedIfEmpty() {
    QSqlQuery q(m_db);
    q.exec("SELECT COUNT(*) FROM users");
    q.next();
    if (q.value(0).toInt() > 0) return;

    auto hash = [](const QString& pw) {
        return QString(QCryptographicHash::hash(pw.toUtf8(), QCryptographicHash::Sha256).toHex());
    };

    QSqlQuery ins(m_db);
    ins.prepare("INSERT INTO users (username, password_hash, role) VALUES (?, ?, ?)");
    ins.bindValue(0, "admin");
    ins.bindValue(1, hash("admin123"));
    ins.bindValue(2, "Admin");
    ins.exec();

    ins.bindValue(0, "registrar");
    ins.bindValue(1, hash("registrar123"));
    ins.bindValue(2, "Registrar");
    ins.exec();

    qInfo() << "Seeded default users: admin/admin123, registrar/registrar123";
}
