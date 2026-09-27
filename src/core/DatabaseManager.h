#pragma once
#include <QSqlDatabase>
#include <QString>

// Singleton wrapper around a SQLite connection. SQLite is used instead of a
// client/server DB so the app runs out-of-the-box with zero setup; swapping
// the driver name + connect options here is enough to move to MySQL/Postgres.
class DatabaseManager {
public:
    static DatabaseManager& instance();

    bool open(const QString& path = QStringLiteral("university.db"));
    QSqlDatabase& db();
    bool isOpen() const;

private:
    DatabaseManager() = default;
    void initSchema();
    void seedIfEmpty();

    QSqlDatabase m_db;
    bool m_open = false;
};
