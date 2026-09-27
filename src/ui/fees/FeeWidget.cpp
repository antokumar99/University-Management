#include "FeeWidget.h"
#include "../../dao/FeeDao.h"
#include "../../dao/StudentDao.h"
#include "../../core/Session.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QSplitter>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>

FeeWidget::FeeWidget(QWidget* parent) : QWidget(parent) {
    // --- Fee structures panel ---
    m_structTable = new QTableWidget(0, 5);
    m_structTable->setHorizontalHeaderLabels({"ID", "Department", "Semester", "Amount", "Description"});
    m_structTable->setColumnHidden(0, true);
    m_structTable->horizontalHeader()->setStretchLastSection(true);
    m_structTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_structTable->setAlternatingRowColors(true);

    m_structDept = new QLineEdit;
    m_structSemester = new QLineEdit;
    m_structAmount = new QDoubleSpinBox;
    m_structAmount->setRange(0, 1000000);
    m_structAmount->setPrefix("$ ");
    m_structDesc = new QLineEdit;

    auto* structForm = new QFormLayout;
    structForm->addRow("Department", m_structDept);
    structForm->addRow("Semester", m_structSemester);
    structForm->addRow("Amount", m_structAmount);
    structForm->addRow("Description", m_structDesc);

    auto* addStructBtn = new QPushButton("Add Fee Structure");
    auto* removeStructBtn = new QPushButton("Remove Selected");
    connect(addStructBtn, &QPushButton::clicked, this, &FeeWidget::addStructure);
    connect(removeStructBtn, &QPushButton::clicked, this, &FeeWidget::removeStructure);

    auto* structBtnRow = new QHBoxLayout;
    structBtnRow->addWidget(addStructBtn);
    structBtnRow->addWidget(removeStructBtn);

    auto* structBox = new QGroupBox("Fee Structures (applied by department)");
    auto* structLayout = new QVBoxLayout(structBox);
    structLayout->addLayout(structForm);
    structLayout->addLayout(structBtnRow);
    structLayout->addWidget(m_structTable);

    // --- Student payments panel ---
    m_studentCombo = new QComboBox;
    connect(m_studentCombo, &QComboBox::currentIndexChanged, this, &FeeWidget::refreshStudentPanel);

    m_paymentsTable = new QTableWidget(0, 3);
    m_paymentsTable->setHorizontalHeaderLabels({"Receipt No", "Date", "Amount"});
    m_paymentsTable->horizontalHeader()->setStretchLastSection(true);
    m_paymentsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_paymentsTable->setAlternatingRowColors(true);

    m_paymentAmount = new QDoubleSpinBox;
    m_paymentAmount->setRange(0, 1000000);
    m_paymentAmount->setPrefix("$ ");
    auto* payBtn = new QPushButton("Record Payment");
    connect(payBtn, &QPushButton::clicked, this, &FeeWidget::recordPayment);

    m_balanceLabel = new QLabel;
    m_balanceLabel->setStyleSheet("font-weight:600;");

    auto* payRow = new QHBoxLayout;
    payRow->addWidget(new QLabel("Amount"));
    payRow->addWidget(m_paymentAmount);
    payRow->addWidget(payBtn);

    auto* payBox = new QGroupBox("Student Payments & Dues");
    auto* payLayout = new QVBoxLayout(payBox);
    payLayout->addWidget(m_studentCombo);
    payLayout->addWidget(m_balanceLabel);
    payLayout->addWidget(m_paymentsTable);
    payLayout->addLayout(payRow);

    auto* splitter = new QSplitter;
    splitter->addWidget(structBox);
    splitter->addWidget(payBox);

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("Fees / Finance"));
    layout->addWidget(splitter);

    loadStudents();
    refreshStructures();
    refreshStudentPanel();
}

void FeeWidget::loadStudents() {
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

void FeeWidget::refreshStructures() {
    FeeDao dao;
    auto list = dao.allStructures();
    m_structTable->setRowCount(list.size());
    for (int i = 0; i < list.size(); ++i) {
        const auto& f = list[i];
        m_structTable->setItem(i, 0, new QTableWidgetItem(QString::number(f.id)));
        m_structTable->setItem(i, 1, new QTableWidgetItem(f.department));
        m_structTable->setItem(i, 2, new QTableWidgetItem(f.semester));
        m_structTable->setItem(i, 3, new QTableWidgetItem(QString::number(f.amount, 'f', 2)));
        m_structTable->setItem(i, 4, new QTableWidgetItem(f.description));
    }
}

void FeeWidget::addStructure() {
    if (m_structDept->text().trimmed().isEmpty() || m_structAmount->value() <= 0) {
        QMessageBox::warning(this, "Missing info", "Department and a positive amount are required.");
        return;
    }
    FeeStructure f;
    f.department = m_structDept->text().trimmed();
    f.semester = m_structSemester->text().trimmed();
    f.amount = m_structAmount->value();
    f.description = m_structDesc->text().trimmed();
    FeeDao dao;
    if (dao.insertStructure(f)) {
        refreshStructures();
        refreshStudentPanel();
    } else {
        QMessageBox::critical(this, "Error", "Could not save fee structure.");
    }
}

void FeeWidget::removeStructure() {
    int row = m_structTable->currentRow();
    if (row < 0) { QMessageBox::information(this, "Select a row", "Choose a fee structure first."); return; }
    int id = m_structTable->item(row, 0)->text().toInt();
    FeeDao dao;
    dao.removeStructure(id);
    refreshStructures();
    refreshStudentPanel();
}

void FeeWidget::refreshStudentPanel() {
    int studentId = m_studentCombo->currentData().toInt();
    if (studentId <= 0) { m_paymentsTable->setRowCount(0); m_balanceLabel->clear(); return; }

    FeeDao dao;
    auto payments = dao.paymentsFor(studentId);
    m_paymentsTable->setRowCount(payments.size());
    for (int i = 0; i < payments.size(); ++i) {
        const auto& p = payments[i];
        m_paymentsTable->setItem(i, 0, new QTableWidgetItem(p.receiptNo));
        m_paymentsTable->setItem(i, 1, new QTableWidgetItem(p.paidAt));
        m_paymentsTable->setItem(i, 2, new QTableWidgetItem(QString::number(p.amountPaid, 'f', 2)));
    }
    double due = dao.totalDueFor(studentId);
    double paid = dao.totalPaidFor(studentId);
    m_balanceLabel->setText(QString("Total Due: $%1   |   Total Paid: $%2   |   Balance: $%3")
        .arg(due, 0, 'f', 2).arg(paid, 0, 'f', 2).arg(due - paid, 0, 'f', 2));
}

void FeeWidget::recordPayment() {
    int studentId = m_studentCombo->currentData().toInt();
    if (studentId <= 0 || m_paymentAmount->value() <= 0) {
        QMessageBox::warning(this, "Missing info", "Select a student and enter a positive amount.");
        return;
    }
    Payment p;
    p.studentId = studentId;
    p.amountPaid = m_paymentAmount->value();
    FeeDao dao;
    if (dao.recordPayment(p)) {
        m_paymentAmount->setValue(0);
        refreshStudentPanel();
        QMessageBox::information(this, "Payment recorded", "Receipt: " + p.receiptNo);
    } else {
        QMessageBox::critical(this, "Error", "Could not record payment.");
    }
}
