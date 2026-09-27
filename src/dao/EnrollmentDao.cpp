#include "EnrollmentDao.h"
#include "../core/DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

static const char* SELECT_JOIN =
    "SELECT e.*, (s.first_name || ' ' || s.last_name) AS student_name, "
    "c.title AS course_title, c.code AS course_code "
    "FROM enrollments e "
    "JOIN students s ON s.id = e.student_id "
    "JOIN courses c ON c.id = e.course_id ";

static Enrollment fromQuery(QSqlQuery& q) {
    Enrollment e;
    e.id = q.value("id").toInt();
    e.studentId = q.value("student_id").toInt();
    e.courseId = q.value("course_id").toInt();
    e.semester = q.value("semester").toString();
    e.status = q.value("status").toString();
    e.studentName = q.value("student_name").toString();
    e.courseTitle = q.value("course_title").toString();
    e.courseCode = q.value("course_code").toString();
    return e;
}

QList<Enrollment> EnrollmentDao::forStudent(int studentId, const QString& semester) {
    QList<Enrollment> out;
    QSqlQuery q(DatabaseManager::instance().db());
    QString sql = QString(SELECT_JOIN) + "WHERE e.student_id=? ";
    if (!semester.isEmpty()) sql += "AND e.semester=? ";
    sql += "ORDER BY e.id DESC";
    q.prepare(sql);
    q.addBindValue(studentId);
    if (!semester.isEmpty()) q.addBindValue(semester);
    q.exec();
    while (q.next()) out.append(fromQuery(q));
    return out;
}

QList<Enrollment> EnrollmentDao::forCourse(int courseId, const QString& semester) {
    QList<Enrollment> out;
    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare(QString(SELECT_JOIN) + "WHERE e.course_id=? AND e.semester=? AND e.status='Registered' "
              "ORDER BY s.first_name");
    q.addBindValue(courseId);
    q.addBindValue(semester);
    q.exec();
    while (q.next()) out.append(fromQuery(q));
    return out;
}

Enrollment EnrollmentDao::byId(int id) {
    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare(QString(SELECT_JOIN) + "WHERE e.id=?");
    q.addBindValue(id);
    q.exec();
    if (q.next()) return fromQuery(q);
    return {};
}

bool EnrollmentDao::alreadyEnrolled(int studentId, int courseId, const QString& semester) {
    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare("SELECT COUNT(*) FROM enrollments WHERE student_id=? AND course_id=? AND semester=? "
              "AND status IN ('Registered','Completed')");
    q.addBindValue(studentId);
    q.addBindValue(courseId);
    q.addBindValue(semester);
    q.exec();
    q.next();
    return q.value(0).toInt() > 0;
}

bool EnrollmentDao::insert(Enrollment& e) {
    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare("INSERT INTO enrollments (student_id, course_id, semester, status) VALUES (?,?,?,?)");
    q.addBindValue(e.studentId);
    q.addBindValue(e.courseId);
    q.addBindValue(e.semester);
    q.addBindValue(e.status);
    if (!q.exec()) { qWarning() << q.lastError().text(); return false; }
    e.id = q.lastInsertId().toInt();
    return true;
}

bool EnrollmentDao::setStatus(int enrollmentId, const QString& status) {
    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare("UPDATE enrollments SET status=? WHERE id=?");
    q.addBindValue(status);
    q.addBindValue(enrollmentId);
    return q.exec();
}
