#include "StudentFormDialog.h"
#include <QFormLayout>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QDateEdit>
#include <QDate>

StudentFormDialog::StudentFormDialog(QWidget* parent, const Student* existing) : QDialog(parent) {
    setWindowTitle(existing ? "Edit Student" : "Admit New Student");
    setMinimumWidth(380);

    m_rollNo = new QLineEdit;
    m_firstName = new QLineEdit;
    m_lastName = new QLineEdit;
    m_email = new QLineEdit;
    m_phone = new QLineEdit;
    m_department = new QLineEdit;
    m_batchYear = new QSpinBox;
    m_batchYear->setRange(2000, 2100);
    m_batchYear->setValue(QDate::currentDate().year());
    m_admissionDate = new QDateEdit(QDate::currentDate());
    m_admissionDate->setCalendarPopup(true);
    m_status = new QComboBox;
    m_status->addItems({"Active", "OnLeave", "Graduated", "Dropped"});

    if (existing) {
        m_id = existing->id;
        m_rollNo->setText(existing->rollNo);
        m_firstName->setText(existing->firstName);
        m_lastName->setText(existing->lastName);
        m_email->setText(existing->email);
        m_phone->setText(existing->phone);
        m_department->setText(existing->department);
        m_batchYear->setValue(existing->batchYear);
        if (!existing->admissionDate.isEmpty())
            m_admissionDate->setDate(QDate::fromString(existing->admissionDate, Qt::ISODate));
        m_status->setCurrentText(existing->status);
    }

    auto* form = new QFormLayout;
    form->addRow("Roll No", m_rollNo);
    form->addRow("First Name", m_firstName);
    form->addRow("Last Name", m_lastName);
    form->addRow("Email", m_email);
    form->addRow("Phone", m_phone);
    form->addRow("Department", m_department);
    form->addRow("Batch Year", m_batchYear);
    form->addRow("Admission Date", m_admissionDate);
    form->addRow("Status", m_status);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto* layout = new QVBoxLayout(this);
    layout->addLayout(form);
    layout->addWidget(buttons);
}

Student StudentFormDialog::result() const {
    Student s;
    s.id = m_id;
    s.rollNo = m_rollNo->text().trimmed();
    s.firstName = m_firstName->text().trimmed();
    s.lastName = m_lastName->text().trimmed();
    s.email = m_email->text().trimmed();
    s.phone = m_phone->text().trimmed();
    s.department = m_department->text().trimmed();
    s.batchYear = m_batchYear->value();
    s.admissionDate = m_admissionDate->date().toString(Qt::ISODate);
    s.status = m_status->currentText();
    return s;
}
