#include "FacultyManagementWidget.h"
#include "FacultyFormDialog.h"
#include "../../dao/FacultyDao.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QLabel>

FacultyManagementWidget::FacultyManagementWidget(QWidget* parent) : QWidget(parent) {
    m_search = new QLineEdit;
    m_search->setPlaceholderText("Search by employee no, name, department...");
    connect(m_search, &QLineEdit::textChanged, this, &FacultyManagementWidget::refresh);

    auto* addBtn = new QPushButton("Add Faculty");
    auto* editBtn = new QPushButton("Edit");
    auto* delBtn = new QPushButton("Remove");
    connect(addBtn, &QPushButton::clicked, this, &FacultyManagementWidget::addFaculty);
    connect(editBtn, &QPushButton::clicked, this, &FacultyManagementWidget::editFaculty);
    connect(delBtn, &QPushButton::clicked, this, &FacultyManagementWidget::deleteFaculty);

    auto* toolbar = new QHBoxLayout;
    toolbar->addWidget(new QLabel("Faculty"));
    toolbar->addStretch();
    toolbar->addWidget(m_search, 1);
    toolbar->addWidget(addBtn);
    toolbar->addWidget(editBtn);
    toolbar->addWidget(delBtn);

    m_table = new QTableWidget(0, 8);
    m_table->setHorizontalHeaderLabels({"ID", "Emp No", "Name", "Dept", "Designation", "Email", "Phone", "Workload (hrs)"});
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

void FacultyManagementWidget::refresh() {
    FacultyDao dao;
    auto list = dao.all(m_search->text());
    m_table->setRowCount(list.size());
    for (int i = 0; i < list.size(); ++i) {
        const Faculty& f = list[i];
        int workload = dao.currentWorkloadHours(f.id);
        m_table->setItem(i, 0, new QTableWidgetItem(QString::number(f.id)));
        m_table->setItem(i, 1, new QTableWidgetItem(f.employeeNo));
        m_table->setItem(i, 2, new QTableWidgetItem(f.fullName()));
        m_table->setItem(i, 3, new QTableWidgetItem(f.department));
        m_table->setItem(i, 4, new QTableWidgetItem(f.designation));
        m_table->setItem(i, 5, new QTableWidgetItem(f.email));
        m_table->setItem(i, 6, new QTableWidgetItem(f.phone));
        auto* loadItem = new QTableWidgetItem(QString("%1 / %2").arg(workload).arg(f.maxLoadHours));
        if (workload > f.maxLoadHours) loadItem->setForeground(Qt::red);
        m_table->setItem(i, 7, loadItem);
    }
}

int FacultyManagementWidget::selectedId() const {
    int row = m_table->currentRow();
    if (row < 0) return -1;
    return m_table->item(row, 0)->text().toInt();
}

void FacultyManagementWidget::addFaculty() {
    FacultyFormDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        Faculty f = dlg.result();
        if (f.employeeNo.isEmpty() || f.firstName.isEmpty()) {
            QMessageBox::warning(this, "Missing info", "Employee No and First Name are required.");
            return;
        }
        FacultyDao dao;
        if (dao.insert(f)) refresh();
        else QMessageBox::critical(this, "Error", "Could not save faculty (employee no may already exist).");
    }
}

void FacultyManagementWidget::editFaculty() {
    int id = selectedId();
    if (id < 0) { QMessageBox::information(this, "Select a row", "Choose a faculty member first."); return; }
    FacultyDao dao;
    Faculty existing = dao.byId(id);
    FacultyFormDialog dlg(this, &existing);
    if (dlg.exec() == QDialog::Accepted) {
        Faculty f = dlg.result();
        if (dao.update(f)) refresh();
        else QMessageBox::critical(this, "Error", "Could not update faculty.");
    }
}

void FacultyManagementWidget::deleteFaculty() {
    int id = selectedId();
    if (id < 0) { QMessageBox::information(this, "Select a row", "Choose a faculty member first."); return; }
    if (QMessageBox::question(this, "Confirm", "Remove this faculty member?") != QMessageBox::Yes) return;
    FacultyDao dao;
    dao.remove(id);
    refresh();
}
