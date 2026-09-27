#pragma once
#include "../models/Models.h"
#include <QList>
#include <QString>

class GradingService {
public:
    // Standard 4.0-scale mapping used across the app.
    static QString letterFor(double marks);
    static double pointFor(const QString& letter);

    // Records marks for an enrollment, deriving letter+point automatically.
    bool recordMarks(int enrollmentId, double marks, QString& errorOut);

    // Semester GPA: credit-weighted average of grade points for one semester.
    double semesterGpa(int studentId, const QString& semester);

    // CGPA: credit-weighted average across every completed/graded semester.
    double cgpa(int studentId);

    QList<GradeRecord> fullTranscript(int studentId);
};
