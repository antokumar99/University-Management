#include "EnrollmentWidget.h"
#include "../../dao/StudentDao.h"
#include "../../dao/CourseDao.h"
#include "../../dao/EnrollmentDao.h"
#include "../../services/EnrollmentService.h"
#include "../../core/Session.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QMessageBox>
#include <QLabel>
#include <QGroupBox>

EnrollmentWidget::EnrollmentWidget(QWidget* parent) : QWidget(parent) {
    m_studentCombo = new QComboBox;
    m_semester = new QLineEdit("Fall2026");
    m_courseCombo = new QComboBox;

    auto* registerBtn = new QPushButton("Register");
    connect(registerBtn, &QPushButton::clicked, this, &EnrollmentWidget::registerCourse);

    auto* form = new QFormLayout;
    form->addRow("Student", m_studentCombo);
    form->addRow("Semester", m_semester);
    form->addRow("Course", m_courseCombo);

    auto* formBox = new QGroupBox("Register for a Course");
    auto* formBoxLayout = new QVBoxLayout(formBox);
    formBoxLayout->addLayout(form);
    formBoxLayout->addWidget(registerBtn);

    auto* dropBtn = new QPushButton("Drop Selected");
    connect(dropBtn, &QPushButton::clicked, this, &EnrollmentWidget::dropSelected);

    m_table = new QTableWidget(0, 5);
    m_table->setHorizontalHeaderLabels({"ID", "Course", "Title", "Semester", "Status"});
    m_table->setColumnHidden(0, true);
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(true);

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(formBox);
    layout->addWidget(new QLabel("Current Enrollments"));
    layout->addWidget(m_table);
    layout->addWidget(dropBtn, 0, Qt::AlignLeft);

    Session& s = Session::instance();
    if (s.role == Role::Student) {
        m_studentCombo->setEnabled(false); // students only manage themselves
    }
    connect(m_studentCombo, &QComboBox::currentIndexChanged, this, &EnrollmentWidget::refreshEnrollments);

    loadStudents();
    loadCourses();
    refreshEnrollments();
}

void EnrollmentWidget::loadStudents() {
    m_studentCombo->clear();
    Session& s = Session::instance();
    StudentDao dao;
    if (s.role == Role::Student) {
        Student me = dao.byId(s.linkedRefId);
        m_studentCombo->addItem(me.fullName() + " (" + me.rollNo + ")", me.id);
        return;
    }
    for (const auto& st : dao.all()) {
        m_studentCombo->addItem(st.fullName() + " (" + st.rollNo + ")", st.id);
    }
}

void EnrollmentWidget::loadCourses() {
    m_courseCombo->clear();
    CourseDao dao;
    for (const auto& c : dao.all()) {
        m_courseCombo->addItem(QString("%1 — %2 (%3 credits)").arg(c.code, c.title).arg(c.creditHours), c.id);
    }
}

int EnrollmentWidget::currentStudentId() const {
    return m_studentCombo->currentData().toInt();
}

void EnrollmentWidget::refreshEnrollments() {
    int studentId = currentStudentId();
    if (studentId <= 0) { m_table->setRowCount(0); return; }
    EnrollmentDao dao;
    auto list = dao.forStudent(studentId);
    m_table->setRowCount(list.size());
    for (int i = 0; i < list.size(); ++i) {
        const Enrollment& e = list[i];
        m_table->setItem(i, 0, new QTableWidgetItem(QString::number(e.id)));
        m_table->setItem(i, 1, new QTableWidgetItem(e.courseCode));
        m_table->setItem(i, 2, new QTableWidgetItem(e.courseTitle));
        m_table->setItem(i, 3, new QTableWidgetItem(e.semester));
        m_table->setItem(i, 4, new QTableWidgetItem(e.status));
    }
}

void EnrollmentWidget::registerCourse() {
    int studentId = currentStudentId();
    int courseId = m_courseCombo->currentData().toInt();
    QString semester = m_semester->text().trimmed();
    if (studentId <= 0 || courseId <= 0 || semester.isEmpty()) {
        QMessageBox::warning(this, "Missing info", "Select a student, course, and semester.");
        return;
    }
    EnrollmentService svc;
    QString error;
    if (svc.registerStudent(studentId, courseId, semester, error)) {
        refreshEnrollments();
        loadCourses(); // seat counts changed
        QMessageBox::information(this, "Registered", "Course registration successful.");
    } else {
        QMessageBox::warning(this, "Registration failed", error);
    }
}

void EnrollmentWidget::dropSelected() {
    int row = m_table->currentRow();
    if (row < 0) { QMessageBox::information(this, "Select a row", "Choose an enrollment to drop."); return; }
    int enrollmentId = m_table->item(row, 0)->text().toInt();
    if (QMessageBox::question(this, "Confirm", "Drop this course?") != QMessageBox::Yes) return;
    EnrollmentService svc;
    QString error;
    if (svc.dropEnrollment(enrollmentId, error)) {
        refreshEnrollments();
        loadCourses();
    } else {
        QMessageBox::warning(this, "Error", error);
    }
}
