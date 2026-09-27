#pragma once
#include <QString>

// Encapsulates the registration business rules: seat limits, duplicate
// enrollment prevention, and prerequisite checking. Keeps this logic out of
// the UI and the DAO (which stay dumb: DAO = SQL, UI = presentation).
class EnrollmentService {
public:
    // Attempts to register studentId into courseId for semester.
    // Returns true on success; on failure fills errorOut with a user-facing reason.
    bool registerStudent(int studentId, int courseId, const QString& semester, QString& errorOut);

    // Drops (soft-cancels) an existing enrollment.
    bool dropEnrollment(int enrollmentId, QString& errorOut);

    // Marks an enrollment Completed (used once grading closes a course for a student).
    bool completeEnrollment(int enrollmentId, QString& errorOut);
};
