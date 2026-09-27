#include "EnrollmentService.h"
#include "../dao/EnrollmentDao.h"
#include "../dao/CourseDao.h"
#include "../dao/GradeDao.h"

bool EnrollmentService::registerStudent(int studentId, int courseId, const QString& semester, QString& errorOut) {
    EnrollmentDao enrollDao;
    CourseDao courseDao;

    if (enrollDao.alreadyEnrolled(studentId, courseId, semester)) {
        errorOut = "Student is already registered in this course for this semester.";
        return false;
    }

    Course c = courseDao.byId(courseId);
    if (c.id < 0) { errorOut = "Course not found."; return false; }

    int taken = courseDao.enrolledCount(courseId, semester);
    if (taken >= c.seatLimit) {
        errorOut = QString("Course is full (%1/%2 seats taken).").arg(taken).arg(c.seatLimit);
        return false;
    }

    // Prerequisite check: every prerequisite course must have a Completed
    // enrollment (any semester) for this student, with a passing grade (point > 0).
    QList<int> prereqs = courseDao.prerequisiteIds(courseId);
    if (!prereqs.isEmpty()) {
        GradeDao gradeDao;
        QList<GradeRecord> transcript = gradeDao.transcriptFor(studentId);
        for (int prereqCourseId : prereqs) {
            Course pc = courseDao.byId(prereqCourseId);
            bool satisfied = false;
            for (const auto& g : transcript) {
                if (g.courseCode == pc.code && g.point > 0.0) { satisfied = true; break; }
            }
            if (!satisfied) {
                errorOut = QString("Missing prerequisite: %1 (%2).").arg(pc.code, pc.title);
                return false;
            }
        }
    }

    Enrollment e;
    e.studentId = studentId;
    e.courseId = courseId;
    e.semester = semester;
    e.status = "Registered";
    if (!enrollDao.insert(e)) {
        errorOut = "Database error while registering.";
        return false;
    }
    return true;
}

bool EnrollmentService::dropEnrollment(int enrollmentId, QString& errorOut) {
    EnrollmentDao dao;
    if (!dao.setStatus(enrollmentId, "Dropped")) {
        errorOut = "Failed to drop course.";
        return false;
    }
    return true;
}

bool EnrollmentService::completeEnrollment(int enrollmentId, QString& errorOut) {
    EnrollmentDao dao;
    if (!dao.setStatus(enrollmentId, "Completed")) {
        errorOut = "Failed to complete enrollment.";
        return false;
    }
    return true;
}
