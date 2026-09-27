#pragma once
#include "../models/Student.h"
#include <QList>
#include <QString>

class StudentDao {
public:
    QList<Student> all(const QString& searchTerm = QString());
    Student byId(int id);
    bool insert(Student& s);      // fills s.id on success
    bool update(const Student& s);
    bool remove(int id);
};
