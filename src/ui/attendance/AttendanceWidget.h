#pragma once
#include <QWidget>

class QComboBox;
class QLineEdit;
class QDateEdit;
class QTableWidget;

class AttendanceWidget : public QWidget {
    Q_OBJECT
public:
    explicit AttendanceWidget(QWidget* parent = nullptr);

private slots:
    void loadRoster();
    void saveAttendance();

private:
    QComboBox* m_courseCombo;
    QLineEdit* m_semester;
    QDateEdit* m_classDate;
    QTableWidget* m_table;

    void loadCourses();
};
