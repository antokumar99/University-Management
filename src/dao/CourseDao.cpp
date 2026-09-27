#include "CourseDao.h"
#include "../core/DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

static const char* SELECT_JOIN =
    "SELECT c.*, (f.first_name || ' ' || f.last_name) AS faculty_name "
    "FROM courses c LEFT JOIN faculty f ON f.id = c.faculty_id ";

static Course fromQuery(QSqlQuery& q) {
    Course c;
    c.id = q.value("id").toInt();
    c.code = q.value("code").toString();
    c.title = q.value("title").toString();
    c.creditHours = q.value("credit_hours").toInt();
    c.department = q.value("department").toString();
    c.semester = q.value("semester").toString();
    c.facultyId = q.value("faculty_id").isNull() ? -1 : q.value("faculty_id").toInt();
    c.facultyName = q.value("faculty_name").toString();
    c.seatLimit = q.value("seat_limit").toInt();
    return c;
}

QList<Course> CourseDao::all(const QString& searchTerm) {
    QList<Course> out;
    QSqlQuery q(DatabaseManager::instance().db());
    QString sql = QString(SELECT_JOIN);
    if (searchTerm.trimmed().isEmpty()) {
        sql += "ORDER BY c.id DESC";
        q.exec(sql);
    } else {
        sql += "WHERE c.code LIKE :t OR c.title LIKE :t OR c.department LIKE :t ORDER BY c.id DESC";
        q.prepare(sql);
        q.bindValue(":t", "%" + searchTerm + "%");
        q.exec();
    }
    while (q.next()) {
        Course c = fromQuery(q);
        c.enrolledCount = enrolledCount(c.id, c.semester);
        out.append(c);
    }
    return out;
}

Course CourseDao::byId(int id) {
    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare(QString(SELECT_JOIN) + "WHERE c.id=?");
    q.addBindValue(id);
    q.exec();
    if (q.next()) return fromQuery(q);
    return {};
}

bool CourseDao::insert(Course& c) {
    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare("INSERT INTO courses (code, title, credit_hours, department, semester, faculty_id, "
              "seat_limit) VALUES (?,?,?,?,?,?,?)");
    q.addBindValue(c.code);
    q.addBindValue(c.title);
    q.addBindValue(c.creditHours);
    q.addBindValue(c.department);
    q.addBindValue(c.semester);
    q.addBindValue(c.facultyId > 0 ? QVariant(c.facultyId) : QVariant(QMetaType(QMetaType::Int)));
    q.addBindValue(c.seatLimit);
    if (!q.exec()) { qWarning() << q.lastError().text(); return false; }
    c.id = q.lastInsertId().toInt();
    return true;
}

bool CourseDao::update(const Course& c) {
    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare("UPDATE courses SET code=?, title=?, credit_hours=?, department=?, semester=?, "
              "faculty_id=?, seat_limit=? WHERE id=?");
    q.addBindValue(c.code);
    q.addBindValue(c.title);
    q.addBindValue(c.creditHours);
    q.addBindValue(c.department);
    q.addBindValue(c.semester);
    q.addBindValue(c.facultyId > 0 ? QVariant(c.facultyId) : QVariant(QMetaType(QMetaType::Int)));
    q.addBindValue(c.seatLimit);
    q.addBindValue(c.id);
    if (!q.exec()) { qWarning() << q.lastError().text(); return false; }
    return true;
}

bool CourseDao::remove(int id) {
    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare("DELETE FROM courses WHERE id=?");
    q.addBindValue(id);
    return q.exec();
}

int CourseDao::enrolledCount(int courseId, const QString& semester) {
    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare("SELECT COUNT(*) FROM enrollments WHERE course_id=? AND semester=? AND status='Registered'");
    q.addBindValue(courseId);
    q.addBindValue(semester);
    q.exec();
    if (q.next()) return q.value(0).toInt();
    return 0;
}

QList<int> CourseDao::prerequisiteIds(int courseId) {
    QList<int> out;
    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare("SELECT prerequisite_course_id FROM prerequisites WHERE course_id=?");
    q.addBindValue(courseId);
    q.exec();
    while (q.next()) out.append(q.value(0).toInt());
    return out;
}

void CourseDao::setPrerequisites(int courseId, const QList<int>& prereqIds) {
    auto& db = DatabaseManager::instance().db();
    QSqlQuery del(db);
    del.prepare("DELETE FROM prerequisites WHERE course_id=?");
    del.addBindValue(courseId);
    del.exec();
    QSqlQuery ins(db);
    ins.prepare("INSERT INTO prerequisites (course_id, prerequisite_course_id) VALUES (?,?)");
    for (int pid : prereqIds) {
        ins.bindValue(0, courseId);
        ins.bindValue(1, pid);
        ins.exec();
    }
}
