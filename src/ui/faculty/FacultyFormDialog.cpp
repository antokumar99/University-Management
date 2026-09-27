#include "FacultyFormDialog.h"
#include <QFormLayout>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QSpinBox>

FacultyFormDialog::FacultyFormDialog(QWidget* parent, const Faculty* existing) : QDialog(parent) {
    setWindowTitle(existing ? "Edit Faculty" : "Add Faculty");
    setMinimumWidth(380);

    m_empNo = new QLineEdit;
    m_first = new QLineEdit;
    m_last = new QLineEdit;
    m_email = new QLineEdit;
    m_phone = new QLineEdit;
    m_dept = new QLineEdit;
    m_designation = new QLineEdit;
    m_maxLoad = new QSpinBox;
    m_maxLoad->setRange(1, 40);
    m_maxLoad->setValue(18);

    if (existing) {
        m_id = existing->id;
        m_empNo->setText(existing->employeeNo);
        m_first->setText(existing->firstName);
        m_last->setText(existing->lastName);
        m_email->setText(existing->email);
        m_phone->setText(existing->phone);
        m_dept->setText(existing->department);
        m_designation->setText(existing->designation);
        m_maxLoad->setValue(existing->maxLoadHours);
    }

    auto* form = new QFormLayout;
    form->addRow("Employee No", m_empNo);
    form->addRow("First Name", m_first);
    form->addRow("Last Name", m_last);
    form->addRow("Email", m_email);
    form->addRow("Phone", m_phone);
    form->addRow("Department", m_dept);
    form->addRow("Designation", m_designation);
    form->addRow("Max Load (credit hrs)", m_maxLoad);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto* layout = new QVBoxLayout(this);
    layout->addLayout(form);
    layout->addWidget(buttons);
}

Faculty FacultyFormDialog::result() const {
    Faculty f;
    f.id = m_id;
    f.employeeNo = m_empNo->text().trimmed();
    f.firstName = m_first->text().trimmed();
    f.lastName = m_last->text().trimmed();
    f.email = m_email->text().trimmed();
    f.phone = m_phone->text().trimmed();
    f.department = m_dept->text().trimmed();
    f.designation = m_designation->text().trimmed();
    f.maxLoadHours = m_maxLoad->value();
    return f;
}
