#pragma once
#include <QString>

struct UserRecord {
    int id = -1;
    QString username;
    QString passwordHash;
    QString role;
    int linkedRefId = -1;
    bool active = true;
    bool found = false;
};

class UserDao {
public:
    UserRecord findByUsername(const QString& username);
    bool createUser(const QString& username, const QString& passwordHash, const QString& role, int linkedRefId = -1);
    bool setPassword(int userId, const QString& newHash);
};
