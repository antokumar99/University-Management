#pragma once
#include <QWidget>

class QComboBox;
class QLineEdit;
class QTableWidget;

class GradingWidget : public QWidget {
    Q_OBJECT
public:
    explicit GradingWidget(QWidget* parent = nullptr);

private slots:
    void loadRoster();
    void saveGrades();

private:
    QComboBox* m_courseCombo;
    QLineEdit* m_semester;
    QTableWidget* m_table;

    void loadCourses();
};
