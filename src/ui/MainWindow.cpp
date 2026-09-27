#include "MainWindow.h"
#include "../core/Session.h"
#include "student/StudentManagementWidget.h"
#include "faculty/FacultyManagementWidget.h"
#include "course/CourseManagementWidget.h"
#include "enrollment/EnrollmentWidget.h"
#include "attendance/AttendanceWidget.h"
#include "exam/GradingWidget.h"
#include "fees/FeeWidget.h"
#include "reports/ReportsWidget.h"
#include "LoginWindow.h"
#include <QTabWidget>
#include <QMenuBar>
#include <QStatusBar>
#include <QLabel>
#include <QMessageBox>
#include <QApplication>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("University Management System");
    resize(1100, 700);

    m_tabs = new QTabWidget;
    setCentralWidget(m_tabs);

    auto* fileMenu = menuBar()->addMenu("&Session");
    auto* logoutAction = fileMenu->addAction("Log Out");
    connect(logoutAction, &QAction::triggered, this, &MainWindow::logout);

    Session& s = Session::instance();
    statusBar()->addPermanentWidget(new QLabel(
        QString("Logged in as %1 (%2)").arg(s.username, roleToString(s.role))));

    buildTabsForRole();
}

void MainWindow::buildTabsForRole() {
    Session& s = Session::instance();

    if (s.can("manage_students"))
        m_tabs->addTab(new StudentManagementWidget, "Students");
    if (s.can("manage_faculty"))
        m_tabs->addTab(new FacultyManagementWidget, "Faculty");
    if (s.can("manage_courses"))
        m_tabs->addTab(new CourseManagementWidget, "Courses");
    if (s.can("manage_enrollment"))
        m_tabs->addTab(new EnrollmentWidget, "Enrollment");
    if (s.can("mark_attendance"))
        m_tabs->addTab(new AttendanceWidget, "Attendance");
    if (s.can("enter_grades"))
        m_tabs->addTab(new GradingWidget, "Examination & Grading");
    if (s.can("manage_fees"))
        m_tabs->addTab(new FeeWidget, "Fees");
    if (s.can("view_reports"))
        m_tabs->addTab(new ReportsWidget, "Reports");

    if (m_tabs->count() == 0) {
        m_tabs->addTab(new QLabel("No modules are available for your role yet."), "Home");
    }
}

void MainWindow::logout() {
    Session::instance().logout();
    close();
    LoginWindow login;
    if (login.exec() == QDialog::Accepted) {
        auto* w = new MainWindow;
        w->show();
    } else {
        qApp->quit();
    }
}
