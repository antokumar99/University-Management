#include "GradingWidget.h"
#include "../../dao/CourseDao.h"
#include "../../dao/GradeDao.h"
#include "../../services/GradingService.h"
#include "../../core/Session.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QMessageBox>
#include <QLabel>

GradingWidget::GradingWidget(QWidget* parent) : QWidget(parent) {
    m_courseCombo = new QComboBox;
    m_semester = new QLineEdit("Fall2026");

    auto* loadBtn = new QPushButton("Load Roster");
    connect(loadBtn, &QPushButton::clicked, this, &GradingWidget::loadRoster);

    auto* form = new QHBoxLayout;
    form->addWidget(new QLabel("Course"));
    form->addWidget(m_courseCombo, 1);
    form->addWidget(new QLabel("Semester"));
    form->addWidget(m_semester);
    form->addWidget(loadBtn);

    m_table = new QTableWidget(0, 5);
    m_table->setHorizontalHeaderLabels({"EnrollmentID", "Student", "Marks (0-100)", "Letter", "Points"});
    m_table->setColumnHidden(0, true);
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->setAlternatingRowColors(true);
    m_table->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);

    auto* saveBtn = new QPushButton("Save Grades");
    connect(saveBtn, &QPushButton::clicked, this, &GradingWidget::saveGrades);

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("Examination & Grading"));
    layout->addLayout(form);
    layout->addWidget(new QLabel("<small>Double-click Marks to edit. Letter and points are computed automatically on save.</small>"));
    layout->addWidget(m_table);
    layout->addWidget(saveBtn, 0, Qt::AlignLeft);

    loadCourses();
    loadRoster();
}

void GradingWidget::loadCourses() {
    m_courseCombo->clear();
    CourseDao dao;
    Session& s = Session::instance();
    for (const auto& c : dao.all()) {
        if (s.role == Role::Faculty && c.facultyId != s.linkedRefId) continue;
        m_courseCombo->addItem(c.code + " — " + c.title, c.id);
    }
}

void GradingWidget::loadRoster() {
    int courseId = m_courseCombo->currentData().toInt();
    QString semester = m_semester->text().trimmed();
    if (courseId <= 0 || semester.isEmpty()) { m_table->setRowCount(0); return; }

    GradeDao dao;
    auto list = dao.forCourseOffering(courseId, semester);
    m_table->setRowCount(list.size());
    for (int i = 0; i < list.size(); ++i) {
        const auto& g = list[i];
        m_table->setItem(i, 0, new QTableWidgetItem(QString::number(g.enrollmentId)));
        auto* nameItem = new QTableWidgetItem(g.studentLabel);
        nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
        m_table->setItem(i, 1, nameItem);
        m_table->setItem(i, 2, new QTableWidgetItem(QString::number(g.marks, 'f', 1)));
        auto* letterItem = new QTableWidgetItem(g.letter);
        letterItem->setFlags(letterItem->flags() & ~Qt::ItemIsEditable);
        m_table->setItem(i, 3, letterItem);
        auto* pointItem = new QTableWidgetItem(QString::number(g.point, 'f', 2));
        pointItem->setFlags(pointItem->flags() & ~Qt::ItemIsEditable);
        m_table->setItem(i, 4, pointItem);
    }
    if (list.isEmpty()) QMessageBox::information(this, "No students", "No registered students for this course/semester.");
}

void GradingWidget::saveGrades() {
    GradingService svc;
    QString error;
    int saved = 0;
    for (int i = 0; i < m_table->rowCount(); ++i) {
        int enrollmentId = m_table->item(i, 0)->text().toInt();
        bool ok = false;
        double marks = m_table->item(i, 2)->text().toDouble(&ok);
        if (!ok) continue;
        marks = qBound(0.0, marks, 100.0);
        if (svc.recordMarks(enrollmentId, marks, error)) {
            saved++;
        }
    }
    loadRoster(); // refresh letters/points from DB
    QMessageBox::information(this, "Saved", QString("Saved grades for %1 student(s).").arg(saved));
}
