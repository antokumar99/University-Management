#pragma once
#include "../models/Models.h"
#include <QList>
#include <QString>

class FacultyDao {
public:
    QList<Faculty> all(const QString& searchTerm = QString());
    Faculty byId(int id);
    bool insert(Faculty& f);
    bool update(const Faculty& f);
    bool remove(int id);
    // sum of credit_hours across courses currently assigned to this faculty
    int currentWorkloadHours(int facultyId);
};
