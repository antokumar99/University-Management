#pragma once
#include <QString>

struct Faculty {
    int id = -1;
    QString employeeNo;
    QString firstName;
    QString lastName;
    QString email;
    QString phone;
    QString department;
    QString designation;
    int maxLoadHours = 18;
    QString fullName() const { return firstName + " " + lastName; }
};

struct Course {
    int id = -1;
    QString code;
    QString title;
    int creditHours = 3;
    QString department;
    QString semester;
    int facultyId = -1;
    QString facultyName; // joined, display only
    int seatLimit = 40;
    int enrolledCount = 0; // computed
};

struct Enrollment {
    int id = -1;
    int studentId = -1;
    int courseId = -1;
    QString semester;
    QString status = "Registered";
    // joined display fields
    QString studentName;
    QString courseTitle;
    QString courseCode;
};

struct GradeRecord {
    int id = -1;
    int enrollmentId = -1;
    double marks = 0.0;
    QString letter;
    double point = 0.0;
    // joined
    QString courseCode;
    QString courseTitle;
    int creditHours = 0;
    QString semester;
    QString studentLabel; // "Name (RollNo)", populated only in course-offering listings
};

struct FeeStructure {
    int id = -1;
    QString department;
    QString semester;
    double amount = 0.0;
    QString description;
};

struct Payment {
    int id = -1;
    int studentId = -1;
    int feeStructureId = -1;
    double amountPaid = 0.0;
    QString paidAt;
    QString receiptNo;
};
