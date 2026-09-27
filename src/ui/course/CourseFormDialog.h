#pragma once
#include <QDialog>
#include "../../models/Models.h"

class QLineEdit;
class QSpinBox;
class QComboBox;
class QListWidget;

class CourseFormDialog : public QDialog {
    Q_OBJECT
public:
    explicit CourseFormDialog(QWidget* parent = nullptr, const Course* existing = nullptr);
    Course result() const;
    QList<int> selectedPrerequisiteIds() const;

private:
    QLineEdit *m_code, *m_title, *m_dept, *m_semester;
    QSpinBox *m_creditHours, *m_seatLimit;
    QComboBox* m_faculty; // stores faculty id in itemData
    QListWidget* m_prereqList; // checkable list of other courses
    int m_id = -1;
};
