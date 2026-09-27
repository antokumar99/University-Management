#pragma once
#include <QDialog>

class QLineEdit;
class QLabel;

class LoginWindow : public QDialog {
    Q_OBJECT
public:
    explicit LoginWindow(QWidget* parent = nullptr);

private slots:
    void attemptLogin();

private:
    QLineEdit* m_username;
    QLineEdit* m_password;
    QLabel* m_error;
};
