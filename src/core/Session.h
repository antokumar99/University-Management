#pragma once
#include <QString>
#include <QHash>
#include <QSet>

enum class Role { Admin, Faculty, Student, Registrar };

inline QString roleToString(Role r) {
    switch (r) {
        case Role::Admin: return "Admin";
        case Role::Faculty: return "Faculty";
        case Role::Student: return "Student";
        case Role::Registrar: return "Registrar";
    }
    return {};
}

inline Role roleFromString(const QString& s) {
    if (s == "Admin") return Role::Admin;
    if (s == "Faculty") return Role::Faculty;
    if (s == "Student") return Role::Student;
    return Role::Registrar;
}

// Holds who is currently logged in for the lifetime of the app session.
// UI code queries this to decide which tabs/actions to show (RBAC).
class Session {
public:
    static Session& instance() {
        static Session s;
        return s;
    }

    int userId = -1;
    QString username;
    Role role = Role::Student;
    int linkedRefId = -1; // student.id or faculty.id, when applicable

    bool isLoggedIn() const { return userId != -1; }

    bool can(const QString& permission) const {
        // Simple capability matrix. Extend as needed.
        static const QHash<QString, QSet<Role>> matrix = {
            {"manage_students",   {Role::Admin, Role::Registrar}},
            {"manage_faculty",    {Role::Admin}},
            {"manage_courses",    {Role::Admin, Role::Registrar}},
            {"manage_enrollment", {Role::Admin, Role::Registrar, Role::Student}},
            {"mark_attendance",   {Role::Admin, Role::Faculty}},
            {"enter_grades",      {Role::Admin, Role::Faculty}},
            {"manage_fees",       {Role::Admin, Role::Registrar}},
            {"view_reports",      {Role::Admin, Role::Registrar, Role::Faculty, Role::Student}},
        };
        return matrix.value(permission).contains(role);
    }

    void logout() { userId = -1; username.clear(); linkedRefId = -1; }

private:
    Session() = default;
};
