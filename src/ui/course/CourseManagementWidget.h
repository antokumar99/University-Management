#pragma once
#include <QWidget>

class QTableWidget;
class QLineEdit;

class CourseManagementWidget : public QWidget {
    Q_OBJECT
public:
    explicit CourseManagementWidget(QWidget* parent = nullptr);

private slots:
    void refresh();
    void addCourse();
    void editCourse();
    void deleteCourse();

private:
    QTableWidget* m_table;
    QLineEdit* m_search;
    int selectedId() const;
};
