#include "FacultyDao.h"
#include "../core/DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

static Faculty fromQuery(QSqlQuery& q) {
    Faculty f;
    f.id = q.value("id").toInt();
    f.employeeNo = q.value("employee_no").toString();
    f.firstName = q.value("first_name").toString();
    f.lastName = q.value("last_name").toString();
    f.email = q.value("email").toString();
    f.phone = q.value("phone").toString();
    f.department = q.value("department").toString();
    f.designation = q.value("designation").toString();
    f.maxLoadHours = q.value("max_load_hours").toInt();
    return f;
}

QList<Faculty> FacultyDao::all(const QString& searchTerm) {
    QList<Faculty> out;
    QSqlQuery q(DatabaseManager::instance().db());
    if (searchTerm.trimmed().isEmpty()) {
        q.exec("SELECT * FROM faculty ORDER BY id DESC");
    } else {
        q.prepare("SELECT * FROM faculty WHERE employee_no LIKE :t OR first_name LIKE :t "
                  "OR last_name LIKE :t OR department LIKE :t ORDER BY id DESC");
        q.bindValue(":t", "%" + searchTerm + "%");
        q.exec();
    }
    while (q.next()) out.append(fromQuery(q));
    return out;
}

Faculty FacultyDao::byId(int id) {
    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare("SELECT * FROM faculty WHERE id=?");
    q.addBindValue(id);
    q.exec();
    if (q.next()) return fromQuery(q);
    return {};
}

bool FacultyDao::insert(Faculty& f) {
    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare("INSERT INTO faculty (employee_no, first_name, last_name, email, phone, department, "
              "designation, max_load_hours) VALUES (?,?,?,?,?,?,?,?)");
    q.addBindValue(f.employeeNo);
    q.addBindValue(f.firstName);
    q.addBindValue(f.lastName);
    q.addBindValue(f.email);
    q.addBindValue(f.phone);
    q.addBindValue(f.department);
    q.addBindValue(f.designation);
    q.addBindValue(f.maxLoadHours);
    if (!q.exec()) { qWarning() << q.lastError().text(); return false; }
    f.id = q.lastInsertId().toInt();
    return true;
}

bool FacultyDao::update(const Faculty& f) {
    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare("UPDATE faculty SET employee_no=?, first_name=?, last_name=?, email=?, phone=?, "
              "department=?, designation=?, max_load_hours=? WHERE id=?");
    q.addBindValue(f.employeeNo);
    q.addBindValue(f.firstName);
    q.addBindValue(f.lastName);
    q.addBindValue(f.email);
    q.addBindValue(f.phone);
    q.addBindValue(f.department);
    q.addBindValue(f.designation);
    q.addBindValue(f.maxLoadHours);
    q.addBindValue(f.id);
    if (!q.exec()) { qWarning() << q.lastError().text(); return false; }
    return true;
}

bool FacultyDao::remove(int id) {
    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare("DELETE FROM faculty WHERE id=?");
    q.addBindValue(id);
    return q.exec();
}

int FacultyDao::currentWorkloadHours(int facultyId) {
    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare("SELECT COALESCE(SUM(credit_hours),0) FROM courses WHERE faculty_id=?");
    q.addBindValue(facultyId);
    q.exec();
    if (q.next()) return q.value(0).toInt();
    return 0;
}
