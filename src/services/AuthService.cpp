#include "AuthService.h"
#include "../dao/UserDao.h"
#include "../core/Session.h"
#include <QCryptographicHash>

QString AuthService::hashPassword(const QString& password) {
    return QString(QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex());
}

bool AuthService::login(const QString& username, const QString& password, QString& errorOut) {
    UserDao dao;
    UserRecord u = dao.findByUsername(username);
    if (!u.found) { errorOut = "No such user."; return false; }
    if (!u.active) { errorOut = "Account is disabled."; return false; }
    if (u.passwordHash != hashPassword(password)) { errorOut = "Incorrect password."; return false; }

    Session& s = Session::instance();
    s.userId = u.id;
    s.username = u.username;
    s.role = roleFromString(u.role);
    s.linkedRefId = u.linkedRefId;
    return true;
}

bool AuthService::registerUser(const QString& username, const QString& password, const QString& role,
                                int linkedRefId, QString& errorOut) {
    UserDao dao;
    if (dao.findByUsername(username).found) { errorOut = "Username already exists."; return false; }
    if (!dao.createUser(username, hashPassword(password), role, linkedRefId)) {
        errorOut = "Failed to create user.";
        return false;
    }
    return true;
}
