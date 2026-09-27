#pragma once
#include <QString>

class AuthService {
public:
    // Attempts login; on success populates Session::instance() and returns true.
    bool login(const QString& username, const QString& password, QString& errorOut);
    bool registerUser(const QString& username, const QString& password, const QString& role,
                       int linkedRefId, QString& errorOut);
    static QString hashPassword(const QString& password);
};
