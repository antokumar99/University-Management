#pragma once
#include "../models/Models.h"
#include <QList>
#include <QString>

class CourseDao {
public:
    QList<Course> all(const QString& searchTerm = QString());
    Course byId(int id);
    bool insert(Course& c);
    bool update(const Course& c);
    bool remove(int id);
    int enrolledCount(int courseId, const QString& semester);
    QList<int> prerequisiteIds(int courseId);
    void setPrerequisites(int courseId, const QList<int>& prereqIds);
};
