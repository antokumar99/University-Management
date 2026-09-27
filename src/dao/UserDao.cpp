#include "UserDao.h"
#include "../core/DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

UserRecord UserDao::findByUsername(const QString& username) {
    UserRecord u;
    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare("SELECT * FROM users WHERE username=?");
    q.addBindValue(username);
    q.exec();
    if (q.next()) {
        u.id = q.value("id").toInt();
        u.username = q.value("username").toString();
        u.passwordHash = q.value("password_hash").toString();
        u.role = q.value("role").toString();
        u.linkedRefId = q.value("linked_ref_id").isNull() ? -1 : q.value("linked_ref_id").toInt();
        u.active = q.value("active").toBool();
        u.found = true;
    }
    return u;
}

bool UserDao::createUser(const QString& username, const QString& passwordHash, const QString& role, int linkedRefId) {
    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare("INSERT INTO users (username, password_hash, role, linked_ref_id) VALUES (?,?,?,?)");
    q.addBindValue(username);
    q.addBindValue(passwordHash);
    q.addBindValue(role);
    q.addBindValue(linkedRefId > 0 ? QVariant(linkedRefId) : QVariant(QMetaType(QMetaType::Int)));
    return q.exec();
}

bool UserDao::setPassword(int userId, const QString& newHash) {
    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare("UPDATE users SET password_hash=? WHERE id=?");
    q.addBindValue(newHash);
    q.addBindValue(userId);
    return q.exec();
}
