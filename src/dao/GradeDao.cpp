#include "GradeDao.h"
#include "../core/DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

bool GradeDao::upsert(int enrollmentId, double marks, const QString& letter, double point) {
    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare("INSERT INTO grades (enrollment_id, marks, grade_letter, grade_point) VALUES (?,?,?,?) "
              "ON CONFLICT(enrollment_id) DO UPDATE SET marks=excluded.marks, "
              "grade_letter=excluded.grade_letter, grade_point=excluded.grade_point");
    q.addBindValue(enrollmentId);
    q.addBindValue(marks);
    q.addBindValue(letter);
    q.addBindValue(point);
    if (!q.exec()) { qWarning() << q.lastError().text(); return false; }
    return true;
}

static GradeRecord fromQuery(QSqlQuery& q) {
    GradeRecord g;
    g.id = q.value("id").toInt();
    g.enrollmentId = q.value("enrollment_id").toInt();
    g.marks = q.value("marks").toDouble();
    g.letter = q.value("grade_letter").toString();
    g.point = q.value("grade_point").toDouble();
    g.courseCode = q.value("code").toString();
    g.courseTitle = q.value("title").toString();
    g.creditHours = q.value("credit_hours").toInt();
    g.semester = q.value("semester").toString();
    return g;
}

QList<GradeRecord> GradeDao::transcriptFor(int studentId) {
    QList<GradeRecord> out;
    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare(
        "SELECT g.*, c.code, c.title, c.credit_hours, e.semester "
        "FROM grades g "
        "JOIN enrollments e ON e.id = g.enrollment_id "
        "JOIN courses c ON c.id = e.course_id "
        "WHERE e.student_id = ? ORDER BY e.semester, c.code");
    q.addBindValue(studentId);
    q.exec();
    while (q.next()) out.append(fromQuery(q));
    return out;
}

QList<GradeRecord> GradeDao::forCourseOffering(int courseId, const QString& semester) {
    QList<GradeRecord> out;
    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare(
        "SELECT COALESCE(g.id,0) AS id, e.id AS enrollment_id, COALESCE(g.marks,0) AS marks, "
        "COALESCE(g.grade_letter,'') AS grade_letter, COALESCE(g.grade_point,0) AS grade_point, "
        "c.code, c.title, c.credit_hours, e.semester, "
        "(s.first_name || ' ' || s.last_name) AS student_name, s.roll_no "
        "FROM enrollments e "
        "JOIN students s ON s.id = e.student_id "
        "JOIN courses c ON c.id = e.course_id "
        "LEFT JOIN grades g ON g.enrollment_id = e.id "
        "WHERE e.course_id=? AND e.semester=? AND e.status='Registered' ORDER BY s.roll_no");
    q.addBindValue(courseId);
    q.addBindValue(semester);
    q.exec();
    while (q.next()) {
        GradeRecord g = fromQuery(q);
        g.studentLabel = q.value("student_name").toString() + " (" + q.value("roll_no").toString() + ")";
        out.append(g);
    }
    return out;
}
