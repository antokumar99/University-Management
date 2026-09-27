#pragma once
#include <QDialog>
#include "../../models/Student.h"

class QLineEdit;
class QComboBox;
class QSpinBox;
class QDateEdit;

class StudentFormDialog : public QDialog {
    Q_OBJECT
public:
    explicit StudentFormDialog(QWidget* parent = nullptr, const Student* existing = nullptr);
    Student result() const;

private:
    QLineEdit* m_rollNo;
    QLineEdit* m_firstName;
    QLineEdit* m_lastName;
    QLineEdit* m_email;
    QLineEdit* m_phone;
    QLineEdit* m_department;
    QSpinBox* m_batchYear;
    QDateEdit* m_admissionDate;
    QComboBox* m_status;
    int m_id = -1;
};
