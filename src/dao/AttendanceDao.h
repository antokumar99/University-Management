#pragma once
#include <QList>
#include <QString>

struct AttendanceRow {
    int enrollmentId;
    QString studentName;
    QString rollNo;
    bool present;
};

class AttendanceDao {
public:
    // one row per registered student in course+semester, with today's/given date's mark if it exists
    QList<AttendanceRow> rosterFor(int courseId, const QString& semester, const QString& classDate);
    void markPresent(int enrollmentId, const QString& classDate, bool present);
    // percentage present out of classes held, for one enrollment
    double attendancePercent(int enrollmentId);
    int classesHeld(int courseId, const QString& semester);
};
