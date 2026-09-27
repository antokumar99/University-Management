#pragma once
#include <QString>

struct Student {
    int id = -1;
    QString rollNo;
    QString firstName;
    QString lastName;
    QString email;
    QString phone;
    QString department;
    int batchYear = 0;
    QString admissionDate;
    QString status = "Active";

    QString fullName() const { return firstName + " " + lastName; }
};
