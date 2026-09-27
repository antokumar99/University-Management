#pragma once
#include <QWidget>

class QComboBox;
class QLineEdit;
class QTableWidget;

class EnrollmentWidget : public QWidget {
    Q_OBJECT
public:
    explicit EnrollmentWidget(QWidget* parent = nullptr);

private slots:
    void refreshEnrollments();
    void registerCourse();
    void dropSelected();

private:
    QComboBox* m_studentCombo; // hidden/locked for Student role
    QLineEdit* m_semester;
    QComboBox* m_courseCombo;
    QTableWidget* m_table;

    int currentStudentId() const;
    void loadStudents();
    void loadCourses();
};
