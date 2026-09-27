#include "GradingService.h"
#include "../dao/GradeDao.h"
#include "../dao/EnrollmentDao.h"
#include "../services/EnrollmentService.h"
#include <QHash>

QString GradingService::letterFor(double marks) {
    if (marks >= 90) return "A+";
    if (marks >= 85) return "A";
    if (marks >= 80) return "A-";
    if (marks >= 75) return "B+";
    if (marks >= 70) return "B";
    if (marks >= 65) return "B-";
    if (marks >= 60) return "C+";
    if (marks >= 55) return "C";
    if (marks >= 50) return "C-";
    if (marks >= 45) return "D";
    return "F";
}

double GradingService::pointFor(const QString& letter) {
    static const QHash<QString, double> table = {
        {"A+", 4.0}, {"A", 4.0}, {"A-", 3.7},
        {"B+", 3.3}, {"B", 3.0}, {"B-", 2.7},
        {"C+", 2.3}, {"C", 2.0}, {"C-", 1.7},
        {"D", 1.0}, {"F", 0.0}
    };
    return table.value(letter, 0.0);
}

bool GradingService::recordMarks(int enrollmentId, double marks, QString& errorOut) {
    GradeDao gradeDao;
    QString letter = letterFor(marks);
    double point = pointFor(letter);
    if (!gradeDao.upsert(enrollmentId, marks, letter, point)) {
        errorOut = "Failed to save grade.";
        return false;
    }
    // Once graded, the enrollment is considered academically complete.
    EnrollmentService es;
    QString ignore;
    es.completeEnrollment(enrollmentId, ignore);
    return true;
}

double GradingService::semesterGpa(int studentId, const QString& semester) {
    GradeDao gradeDao;
    QList<GradeRecord> all = gradeDao.transcriptFor(studentId);
    double weightedSum = 0.0;
    int totalCredits = 0;
    for (const auto& g : all) {
        if (g.semester != semester) continue;
        weightedSum += g.point * g.creditHours;
        totalCredits += g.creditHours;
    }
    if (totalCredits == 0) return 0.0;
    return weightedSum / totalCredits;
}

double GradingService::cgpa(int studentId) {
    GradeDao gradeDao;
    QList<GradeRecord> all = gradeDao.transcriptFor(studentId);
    double weightedSum = 0.0;
    int totalCredits = 0;
    for (const auto& g : all) {
        weightedSum += g.point * g.creditHours;
        totalCredits += g.creditHours;
    }
    if (totalCredits == 0) return 0.0;
    return weightedSum / totalCredits;
}

QList<GradeRecord> GradingService::fullTranscript(int studentId) {
    GradeDao gradeDao;
    return gradeDao.transcriptFor(studentId);
}
