#include "LoginWindow.h"
#include "../services/AuthService.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

LoginWindow::LoginWindow(QWidget* parent) : QDialog(parent) {
    setWindowTitle("University Management System — Login");
    setMinimumWidth(360);

    auto* title = new QLabel("University Management System");
    title->setStyleSheet("font-size: 18px; font-weight: 600;");
    auto* subtitle = new QLabel("Sign in to continue");
    subtitle->setStyleSheet("color: #666; margin-bottom: 12px;");

    m_username = new QLineEdit;
    m_username->setPlaceholderText("Username");
    m_password = new QLineEdit;
    m_password->setPlaceholderText("Password");
    m_password->setEchoMode(QLineEdit::Password);

    auto* form = new QFormLayout;
    form->addRow("Username", m_username);
    form->addRow("Password", m_password);

    m_error = new QLabel;
    m_error->setStyleSheet("color: #c0392b;");
    m_error->setVisible(false);

    auto* loginBtn = new QPushButton("Log In");
    loginBtn->setDefault(true);
    connect(loginBtn, &QPushButton::clicked, this, &LoginWindow::attemptLogin);
    connect(m_password, &QLineEdit::returnPressed, this, &LoginWindow::attemptLogin);

    auto* hint = new QLabel("<small>Default admin: admin / admin123</small>");
    hint->setStyleSheet("color:#999;");

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(title);
    layout->addWidget(subtitle);
    layout->addLayout(form);
    layout->addWidget(m_error);
    layout->addWidget(loginBtn);
    layout->addWidget(hint);
}

void LoginWindow::attemptLogin() {
    AuthService auth;
    QString error;
    if (auth.login(m_username->text().trimmed(), m_password->text(), error)) {
        accept();
    } else {
        m_error->setText(error);
        m_error->setVisible(true);
    }
}
