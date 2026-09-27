#include <QApplication>
#include "core/DatabaseManager.h"
#include "ui/LoginWindow.h"
#include "ui/MainWindow.h"
#include <QMessageBox>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QApplication::setApplicationName("University Management System");
    QApplication::setOrganizationName("UMS");

    if (!DatabaseManager::instance().open()) {
        QMessageBox::critical(nullptr, "Database Error",
            "Could not open the local database file (university.db). The app will now exit.");
        return 1;
    }

    LoginWindow login;
    if (login.exec() != QDialog::Accepted) {
        return 0; // user closed the login dialog without logging in
    }

    MainWindow window;
    window.show();

    return app.exec();
}
