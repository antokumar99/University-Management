#pragma once
#include <QWidget>
#include "../../models/Student.h"

class QTableWidget;
class QLineEdit;

class StudentManagementWidget : public QWidget {
    Q_OBJECT
public:
    explicit StudentManagementWidget(QWidget* parent = nullptr);

private slots:
    void refresh();
    void addStudent();
    void editStudent();
    void deleteStudent();

private:
    QTableWidget* m_table;
    QLineEdit* m_search;
    void populateTable(const QList<Student>& students);
    int selectedStudentId() const;
};
