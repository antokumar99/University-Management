#include "ReportsWidget.h"
#include "../../dao/StudentDao.h"
#include "../../dao/CourseDao.h"
#include "../../services/ReportService.h"
#include "../../core/Session.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QLineEdit>
#include <QDateEdit>
#include <QPushButton>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QDate>

ReportsWidget::ReportsWidget(QWidget* parent) : QWidget(parent) {
    // Transcript
    m_transcriptStudent = new QComboBox;
    auto* transcriptBtn = new QPushButton("Export Transcript (PDF)");
    connect(transcriptBtn, &QPushButton::clicked, this, &ReportsWidget::exportTranscript);
    auto* transcriptBox = new QGroupBox("Academic Transcript");
    auto* transcriptLayout = new QVBoxLayout(transcriptBox);
    transcriptLayout->addWidget(new QLabel("Student"));
    transcriptLayout->addWidget(m_transcriptStudent);
    transcriptLayout->addWidget(transcriptBtn, 0, Qt::AlignLeft);

    // Attendance sheet
    m_attCourse = new QComboBox;
    m_attSemester = new QLineEdit("Fall2026");
    m_attDate = new QDateEdit(QDate::currentDate());
    m_attDate->setCalendarPopup(true);
    auto* attBtn = new QPushButton("Export Attendance Sheet (PDF)");
    connect(attBtn, &QPushButton::clicked, this, &ReportsWidget::exportAttendance);
    auto* attRow = new QHBoxLayout;
    attRow->addWidget(new QLabel("Course"));
    attRow->addWidget(m_attCourse, 1);
    attRow->addWidget(new QLabel("Semester"));
    attRow->addWidget(m_attSemester);
    attRow->addWidget(new QLabel("Date"));
    attRow->addWidget(m_attDate);
    auto* attBox = new QGroupBox("Attendance Sheet");
    auto* attLayout = new QVBoxLayout(attBox);
    attLayout->addLayout(attRow);
    attLayout->addWidget(attBtn, 0, Qt::AlignLeft);

    // Fee statement
    m_feeStudent = new QComboBox;
    auto* feeBtn = new QPushButton("Export Fee Statement (PDF)");
    connect(feeBtn, &QPushButton::clicked, this, &ReportsWidget::exportFeeStatement);
    auto* feeBox = new QGroupBox("Fee Statement");
    auto* feeLayout = new QVBoxLayout(feeBox);
    feeLayout->addWidget(new QLabel("Student"));
    feeLayout->addWidget(m_feeStudent);
    feeLayout->addWidget(feeBtn, 0, Qt::AlignLeft);

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("Reports"));
    layout->addWidget(transcriptBox);
    layout->addWidget(attBox);
    layout->addWidget(feeBox);
    layout->addStretch();

    loadStudentCombos();
    loadCourseCombo();
}

void ReportsWidget::loadStudentCombos() {
    m_transcriptStudent->clear();
    m_feeStudent->clear();
    Session& s = Session::instance();
    StudentDao dao;
    if (s.role == Role::Student) {
        Student me = dao.byId(s.linkedRefId);
        QString label = me.fullName() + " (" + me.rollNo + ")";
        m_transcriptStudent->addItem(label, me.id);
        m_feeStudent->addItem(label, me.id);
        return;
    }
    for (const auto& st : dao.all()) {
        QString label = st.fullName() + " (" + st.rollNo + ")";
        m_transcriptStudent->addItem(label, st.id);
        m_feeStudent->addItem(label, st.id);
    }
}

void ReportsWidget::loadCourseCombo() {
    m_attCourse->clear();
    CourseDao dao;
    Session& s = Session::instance();
    for (const auto& c : dao.all()) {
        if (s.role == Role::Faculty && c.facultyId != s.linkedRefId) continue;
        m_attCourse->addItem(c.code + " — " + c.title, c.id);
    }
}

QString ReportsWidget::pickSavePath(const QString& suggestedName) {
    return QFileDialog::getSaveFileName(this, "Save PDF", suggestedName, "PDF Files (*.pdf)");
}

void ReportsWidget::exportTranscript() {
    int studentId = m_transcriptStudent->currentData().toInt();
    if (studentId <= 0) return;
    QString path = pickSavePath("transcript.pdf");
    if (path.isEmpty()) return;
    ReportService svc;
    QString error;
    if (svc.exportTranscript(studentId, path, error)) {
        QMessageBox::information(this, "Done", "Transcript saved to " + path);
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
    } else {
        QMessageBox::critical(this, "Error", error);
    }
}

void ReportsWidget::exportAttendance() {
    int courseId = m_attCourse->currentData().toInt();
    QString semester = m_attSemester->text().trimmed();
    QString date = m_attDate->date().toString(Qt::ISODate);
    if (courseId <= 0 || semester.isEmpty()) return;
    QString path = pickSavePath("attendance.pdf");
    if (path.isEmpty()) return;
    ReportService svc;
    QString error;
    if (svc.exportAttendanceSheet(courseId, semester, date, path, error)) {
        QMessageBox::information(this, "Done", "Attendance sheet saved to " + path);
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
    } else {
        QMessageBox::critical(this, "Error", error);
    }
}

void ReportsWidget::exportFeeStatement() {
    int studentId = m_feeStudent->currentData().toInt();
    if (studentId <= 0) return;
    QString path = pickSavePath("fee_statement.pdf");
    if (path.isEmpty()) return;
    ReportService svc;
    QString error;
    if (svc.exportFeeStatement(studentId, path, error)) {
        QMessageBox::information(this, "Done", "Fee statement saved to " + path);
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
    } else {
        QMessageBox::critical(this, "Error", error);
    }
}
