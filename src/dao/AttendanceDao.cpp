#include "AttendanceDao.h"
#include "../core/DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

QList<AttendanceRow> AttendanceDao::rosterFor(int courseId, const QString& semester, const QString& classDate) {
    QList<AttendanceRow> out;
    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare(
        "SELECT e.id AS enrollment_id, s.roll_no, (s.first_name || ' ' || s.last_name) AS name, "
        "COALESCE(a.present, 1) AS present "
        "FROM enrollments e "
        "JOIN students s ON s.id = e.student_id "
        "LEFT JOIN attendance a ON a.enrollment_id = e.id AND a.class_date = :d "
        "WHERE e.course_id = :c AND e.semester = :s AND e.status = 'Registered' "
        "ORDER BY s.roll_no");
    q.bindValue(":d", classDate);
    q.bindValue(":c", courseId);
    q.bindValue(":s", semester);
    if (!q.exec()) qWarning() << q.lastError().text();
    while (q.next()) {
        AttendanceRow r;
        r.enrollmentId = q.value("enrollment_id").toInt();
        r.rollNo = q.value("roll_no").toString();
        r.studentName = q.value("name").toString();
        r.present = q.value("present").toBool();
        out.append(r);
    }
    return out;
}

void AttendanceDao::markPresent(int enrollmentId, const QString& classDate, bool present) {
    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare("INSERT INTO attendance (enrollment_id, class_date, present) VALUES (?,?,?) "
              "ON CONFLICT(enrollment_id, class_date) DO UPDATE SET present=excluded.present");
    q.addBindValue(enrollmentId);
    q.addBindValue(classDate);
    q.addBindValue(present ? 1 : 0);
    if (!q.exec()) qWarning() << q.lastError().text();
}

double AttendanceDao::attendancePercent(int enrollmentId) {
    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare("SELECT COUNT(*), SUM(present) FROM attendance WHERE enrollment_id=?");
    q.addBindValue(enrollmentId);
    q.exec();
    if (q.next()) {
        int total = q.value(0).toInt();
        int present = q.value(1).toInt();
        if (total == 0) return 100.0;
        return (double)present / total * 100.0;
    }
    return 0.0;
}

int AttendanceDao::classesHeld(int courseId, const QString& semester) {
    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare("SELECT COUNT(DISTINCT a.class_date) FROM attendance a "
              "JOIN enrollments e ON e.id = a.enrollment_id "
              "WHERE e.course_id=? AND e.semester=?");
    q.addBindValue(courseId);
    q.addBindValue(semester);
    q.exec();
    if (q.next()) return q.value(0).toInt();
    return 0;
}
