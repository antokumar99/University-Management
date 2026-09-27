#pragma once
#include "../models/Models.h"
#include <QList>

class GradeDao {
public:
    bool upsert(int enrollmentId, double marks, const QString& letter, double point);
    // full transcript for a student: all graded enrollments across semesters
    QList<GradeRecord> transcriptFor(int studentId);
    // grades for one course+semester (for a faculty's grade-entry screen)
    QList<GradeRecord> forCourseOffering(int courseId, const QString& semester);
};
