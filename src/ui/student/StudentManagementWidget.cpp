#include "StudentManagementWidget.h"
#include "StudentFormDialog.h"
#include "../../dao/StudentDao.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QLabel>

StudentManagementWidget::StudentManagementWidget(QWidget* parent) : QWidget(parent) {
    m_search = new QLineEdit;
    m_search->setPlaceholderText("Search by roll no, name, department...");
    connect(m_search, &QLineEdit::textChanged, this, &StudentManagementWidget::refresh);

    auto* addBtn = new QPushButton("Admit Student");
    auto* editBtn = new QPushButton("Edit");
    auto* delBtn = new QPushButton("Remove");
    connect(addBtn, &QPushButton::clicked, this, &StudentManagementWidget::addStudent);
    connect(editBtn, &QPushButton::clicked, this, &StudentManagementWidget::editStudent);
    connect(delBtn, &QPushButton::clicked, this, &StudentManagementWidget::deleteStudent);

    auto* toolbar = new QHBoxLayout;
    toolbar->addWidget(new QLabel("Students"));
    toolbar->addStretch();
    toolbar->addWidget(m_search, 1);
    toolbar->addWidget(addBtn);
    toolbar->addWidget(editBtn);
    toolbar->addWidget(delBtn);

    m_table = new QTableWidget(0, 8);
    m_table->setHorizontalHeaderLabels({"ID", "Roll No", "Name", "Dept", "Batch", "Email", "Phone", "Status"});
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

void StudentManagementWidget::refresh() {
    StudentDao dao;
    populateTable(dao.all(m_search->text()));
}

void StudentManagementWidget::populateTable(const QList<Student>& students) {
    m_table->setRowCount(students.size());
    for (int i = 0; i < students.size(); ++i) {
        const Student& s = students[i];
        m_table->setItem(i, 0, new QTableWidgetItem(QString::number(s.id)));
        m_table->setItem(i, 1, new QTableWidgetItem(s.rollNo));
        m_table->setItem(i, 2, new QTableWidgetItem(s.fullName()));
        m_table->setItem(i, 3, new QTableWidgetItem(s.department));
        m_table->setItem(i, 4, new QTableWidgetItem(QString::number(s.batchYear)));
        m_table->setItem(i, 5, new QTableWidgetItem(s.email));
        m_table->setItem(i, 6, new QTableWidgetItem(s.phone));
        m_table->setItem(i, 7, new QTableWidgetItem(s.status));
    }
}

int StudentManagementWidget::selectedStudentId() const {
    int row = m_table->currentRow();
    if (row < 0) return -1;
    return m_table->item(row, 0)->text().toInt();
}

void StudentManagementWidget::addStudent() {
    StudentFormDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        Student s = dlg.result();
        if (s.rollNo.isEmpty() || s.firstName.isEmpty()) {
            QMessageBox::warning(this, "Missing info", "Roll No and First Name are required.");
            return;
        }
        StudentDao dao;
        if (dao.insert(s)) refresh();
        else QMessageBox::critical(this, "Error", "Could not save student (roll no may already exist).");
    }
}

void StudentManagementWidget::editStudent() {
    int id = selectedStudentId();
    if (id < 0) { QMessageBox::information(this, "Select a student", "Choose a row first."); return; }
    StudentDao dao;
    Student existing = dao.byId(id);
    StudentFormDialog dlg(this, &existing);
    if (dlg.exec() == QDialog::Accepted) {
        Student s = dlg.result();
        if (dao.update(s)) refresh();
        else QMessageBox::critical(this, "Error", "Could not update student.");
    }
}

void StudentManagementWidget::deleteStudent() {
    int id = selectedStudentId();
    if (id < 0) { QMessageBox::information(this, "Select a student", "Choose a row first."); return; }
    if (QMessageBox::question(this, "Confirm", "Remove this student and all related records?")
        != QMessageBox::Yes) return;
    StudentDao dao;
    dao.remove(id);
    refresh();
}
