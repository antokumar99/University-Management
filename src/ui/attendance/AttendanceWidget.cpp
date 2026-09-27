#include "AttendanceWidget.h"
#include "../../dao/CourseDao.h"
#include "../../dao/AttendanceDao.h"
#include "../../core/Session.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QComboBox>
#include <QLineEdit>
#include <QDateEdit>
#include <QDate>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QMessageBox>
#include <QLabel>

AttendanceWidget::AttendanceWidget(QWidget* parent) : QWidget(parent) {
    m_courseCombo = new QComboBox;
    m_semester = new QLineEdit("Fall2026");
    m_classDate = new QDateEdit(QDate::currentDate());
    m_classDate->setCalendarPopup(true);

    auto* loadBtn = new QPushButton("Load Roster");
    connect(loadBtn, &QPushButton::clicked, this, &AttendanceWidget::loadRoster);

    auto* form = new QHBoxLayout;
    form->addWidget(new QLabel("Course"));
    form->addWidget(m_courseCombo, 1);
    form->addWidget(new QLabel("Semester"));
    form->addWidget(m_semester);
    form->addWidget(new QLabel("Date"));
    form->addWidget(m_classDate);
    form->addWidget(loadBtn);

    m_table = new QTableWidget(0, 4);
    m_table->setHorizontalHeaderLabels({"EnrollmentID", "Roll No", "Student", "Present"});
    m_table->setColumnHidden(0, true);
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->setAlternatingRowColors(true);

    auto* saveBtn = new QPushButton("Save Attendance");
    connect(saveBtn, &QPushButton::clicked, this, &AttendanceWidget::saveAttendance);

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("Attendance"));
    layout->addLayout(form);
    layout->addWidget(m_table);
    layout->addWidget(saveBtn, 0, Qt::AlignLeft);

    loadCourses();
    loadRoster();
}

void AttendanceWidget::loadCourses() {
    m_courseCombo->clear();
    CourseDao dao;
    Session& s = Session::instance();
    for (const auto& c : dao.all()) {
        if (s.role == Role::Faculty && c.facultyId != s.linkedRefId) continue; // faculty sees only their courses
        m_courseCombo->addItem(c.code + " — " + c.title, c.id);
    }
}

void AttendanceWidget::loadRoster() {
    int courseId = m_courseCombo->currentData().toInt();
    QString semester = m_semester->text().trimmed();
    QString date = m_classDate->date().toString(Qt::ISODate);
    if (courseId <= 0 || semester.isEmpty()) { m_table->setRowCount(0); return; }

    AttendanceDao dao;
    auto roster = dao.rosterFor(courseId, semester, date);
    m_table->setRowCount(roster.size());
    for (int i = 0; i < roster.size(); ++i) {
        const auto& r = roster[i];
        m_table->setItem(i, 0, new QTableWidgetItem(QString::number(r.enrollmentId)));
        m_table->setItem(i, 1, new QTableWidgetItem(r.rollNo));
        m_table->setItem(i, 2, new QTableWidgetItem(r.studentName));
        auto* presentItem = new QTableWidgetItem();
        presentItem->setFlags(presentItem->flags() | Qt::ItemIsUserCheckable);
        presentItem->setCheckState(r.present ? Qt::Checked : Qt::Unchecked);
        m_table->setItem(i, 3, presentItem);
    }
    if (roster.isEmpty()) QMessageBox::information(this, "No students", "No registered students for this course/semester.");
}

void AttendanceWidget::saveAttendance() {
    QString date = m_classDate->date().toString(Qt::ISODate);
    AttendanceDao dao;
    for (int i = 0; i < m_table->rowCount(); ++i) {
        int enrollmentId = m_table->item(i, 0)->text().toInt();
        bool present = m_table->item(i, 3)->checkState() == Qt::Checked;
        dao.markPresent(enrollmentId, date, present);
    }
    QMessageBox::information(this, "Saved", "Attendance saved for " + date + ".");
}
