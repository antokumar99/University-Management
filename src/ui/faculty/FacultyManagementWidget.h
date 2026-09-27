#pragma once
#include <QWidget>

class QTableWidget;
class QLineEdit;

class FacultyManagementWidget : public QWidget {
    Q_OBJECT
public:
    explicit FacultyManagementWidget(QWidget* parent = nullptr);

private slots:
    void refresh();
    void addFaculty();
    void editFaculty();
    void deleteFaculty();

private:
    QTableWidget* m_table;
    QLineEdit* m_search;
    int selectedId() const;
};
