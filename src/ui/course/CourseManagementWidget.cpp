#include "CourseManagementWidget.h"
#include "CourseFormDialog.h"
#include "../../dao/CourseDao.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QLabel>

CourseManagementWidget::CourseManagementWidget(QWidget* parent) : QWidget(parent) {
    m_search = new QLineEdit;
    m_search->setPlaceholderText("Search by code, title, department...");
    connect(m_search, &QLineEdit::textChanged, this, &CourseManagementWidget::refresh);

    auto* addBtn = new QPushButton("Add Course");
    auto* editBtn = new QPushButton("Edit");
    auto* delBtn = new QPushButton("Remove");
    connect(addBtn, &QPushButton::clicked, this, &CourseManagementWidget::addCourse);
    connect(editBtn, &QPushButton::clicked, this, &CourseManagementWidget::editCourse);
    connect(delBtn, &QPushButton::clicked, this, &CourseManagementWidget::deleteCourse);

    auto* toolbar = new QHBoxLayout;
    toolbar->addWidget(new QLabel("Courses & Curriculum"));
    toolbar->addStretch();
    toolbar->addWidget(m_search, 1);
    toolbar->addWidget(addBtn);
    toolbar->addWidget(editBtn);
    toolbar->addWidget(delBtn);

    m_table = new QTableWidget(0, 9);
    m_table->setHorizontalHeaderLabels({"ID", "Code", "Title", "Dept", "Semester", "Credits", "Faculty", "Seats Taken", "Seat Limit"});
    m_table->setColumnHidden(0, true);
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(true);

    auto* layout = new QVBoxLayout(this);
    layout->addLayout(toolbar);
    layout->addWidget(m_table);

    refresh();
}

void CourseManagementWidget::refresh() {
    CourseDao dao;
    auto list = dao.all(m_search->text());
    m_table->setRowCount(list.size());
    for (int i = 0; i < list.size(); ++i) {
        const Course& c = list[i];
        m_table->setItem(i, 0, new QTableWidgetItem(QString::number(c.id)));
        m_table->setItem(i, 1, new QTableWidgetItem(c.code));
        m_table->setItem(i, 2, new QTableWidgetItem(c.title));
        m_table->setItem(i, 3, new QTableWidgetItem(c.department));
        m_table->setItem(i, 4, new QTableWidgetItem(c.semester));
        m_table->setItem(i, 5, new QTableWidgetItem(QString::number(c.creditHours)));
        m_table->setItem(i, 6, new QTableWidgetItem(c.facultyName.isEmpty() ? "(unassigned)" : c.facultyName));
        auto* seatsItem = new QTableWidgetItem(QString::number(c.enrolledCount));
        if (c.enrolledCount >= c.seatLimit) seatsItem->setForeground(Qt::red);
        m_table->setItem(i, 7, seatsItem);
        m_table->setItem(i, 8, new QTableWidgetItem(QString::number(c.seatLimit)));
    }
}

int CourseManagementWidget::selectedId() const {
    int row = m_table->currentRow();
    if (row < 0) return -1;
    return m_table->item(row, 0)->text().toInt();
}

void CourseManagementWidget::addCourse() {
    CourseFormDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        Course c = dlg.result();
        if (c.code.isEmpty() || c.title.isEmpty()) {
            QMessageBox::warning(this, "Missing info", "Course Code and Title are required.");
            return;
        }
        CourseDao dao;
        if (dao.insert(c)) {
            dao.setPrerequisites(c.id, dlg.selectedPrerequisiteIds());
            refresh();
        } else {
            QMessageBox::critical(this, "Error", "Could not save course (code may already exist).");
        }
    }
}

void CourseManagementWidget::editCourse() {
    int id = selectedId();
    if (id < 0) { QMessageBox::information(this, "Select a row", "Choose a course first."); return; }
    CourseDao dao;
    Course existing = dao.byId(id);
    CourseFormDialog dlg(this, &existing);
    if (dlg.exec() == QDialog::Accepted) {
        Course c = dlg.result();
        if (dao.update(c)) {
            dao.setPrerequisites(c.id, dlg.selectedPrerequisiteIds());
            refresh();
        } else {
            QMessageBox::critical(this, "Error", "Could not update course.");
        }
    }
}

void CourseManagementWidget::deleteCourse() {
    int id = selectedId();
    if (id < 0) { QMessageBox::information(this, "Select a row", "Choose a course first."); return; }
    if (QMessageBox::question(this, "Confirm", "Remove this course and its enrollments?") != QMessageBox::Yes) return;
    CourseDao dao;
    dao.remove(id);
    refresh();
}
