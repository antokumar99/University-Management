#include "StudentDao.h"
#include "../core/DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

static Student fromQuery(QSqlQuery& q) {
    Student s;
    s.id = q.value("id").toInt();
    s.rollNo = q.value("roll_no").toString();
    s.firstName = q.value("first_name").toString();
    s.lastName = q.value("last_name").toString();
    s.email = q.value("email").toString();
    s.phone = q.value("phone").toString();
    s.department = q.value("department").toString();
    s.batchYear = q.value("batch_year").toInt();
    s.admissionDate = q.value("admission_date").toString();
    s.status = q.value("status").toString();
    return s;
}

QList<Student> StudentDao::all(const QString& searchTerm) {
    QList<Student> out;
    QSqlQuery q(DatabaseManager::instance().db());
    if (searchTerm.trimmed().isEmpty()) {
        q.exec("SELECT * FROM students ORDER BY id DESC");
    } else {
        q.prepare("SELECT * FROM students WHERE roll_no LIKE :t OR first_name LIKE :t "
                  "OR last_name LIKE :t OR department LIKE :t ORDER BY id DESC");
        q.bindValue(":t", "%" + searchTerm + "%");
        q.exec();
    }
    while (q.next()) out.append(fromQuery(q));
    return out;
}

Student StudentDao::byId(int id) {
    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare("SELECT * FROM students WHERE id = ?");
    q.addBindValue(id);
    q.exec();
    if (q.next()) return fromQuery(q);
    return {};
}

bool StudentDao::insert(Student& s) {
    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare("INSERT INTO students (roll_no, first_name, last_name, email, phone, department, "
              "batch_year, admission_date, status) VALUES (?,?,?,?,?,?,?,?,?)");
    q.addBindValue(s.rollNo);
    q.addBindValue(s.firstName);
    q.addBindValue(s.lastName);
    q.addBindValue(s.email);
    q.addBindValue(s.phone);
    q.addBindValue(s.department);
    q.addBindValue(s.batchYear);
    q.addBindValue(s.admissionDate);
    q.addBindValue(s.status);
    if (!q.exec()) { qWarning() << q.lastError().text(); return false; }
    s.id = q.lastInsertId().toInt();
    return true;
}

bool StudentDao::update(const Student& s) {
    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare("UPDATE students SET roll_no=?, first_name=?, last_name=?, email=?, phone=?, "
              "department=?, batch_year=?, admission_date=?, status=? WHERE id=?");
    q.addBindValue(s.rollNo);
    q.addBindValue(s.firstName);
    q.addBindValue(s.lastName);
    q.addBindValue(s.email);
    q.addBindValue(s.phone);
    q.addBindValue(s.department);
    q.addBindValue(s.batchYear);
    q.addBindValue(s.admissionDate);
    q.addBindValue(s.status);
    q.addBindValue(s.id);
    if (!q.exec()) { qWarning() << q.lastError().text(); return false; }
    return true;
}

bool StudentDao::remove(int id) {
    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare("DELETE FROM students WHERE id=?");
    q.addBindValue(id);
    return q.exec();
}
