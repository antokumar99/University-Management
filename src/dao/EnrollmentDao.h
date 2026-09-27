#pragma once
#include "../models/Models.h"
#include <QList>
#include <QString>

class EnrollmentDao {
public:
    QList<Enrollment> forStudent(int studentId, const QString& semester = QString());
    QList<Enrollment> forCourse(int courseId, const QString& semester);
    Enrollment byId(int id);
    bool alreadyEnrolled(int studentId, int courseId, const QString& semester);
    bool insert(Enrollment& e);
    bool setStatus(int enrollmentId, const QString& status);
};
