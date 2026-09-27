#pragma once
#include <QDialog>
#include "../../models/Models.h"

class QLineEdit;
class QSpinBox;

class FacultyFormDialog : public QDialog {
    Q_OBJECT
public:
    explicit FacultyFormDialog(QWidget* parent = nullptr, const Faculty* existing = nullptr);
    Faculty result() const;

private:
    QLineEdit *m_empNo, *m_first, *m_last, *m_email, *m_phone, *m_dept, *m_designation;
    QSpinBox* m_maxLoad;
    int m_id = -1;
};
