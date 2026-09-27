#pragma once
#include <QWidget>

class QComboBox;
class QTableWidget;
class QLineEdit;
class QLabel;
class QDoubleSpinBox;

class FeeWidget : public QWidget {
    Q_OBJECT
public:
    explicit FeeWidget(QWidget* parent = nullptr);

private slots:
    void refreshStructures();
    void addStructure();
    void removeStructure();
    void refreshStudentPanel();
    void recordPayment();

private:
    // fee structures
    QTableWidget* m_structTable;
    QLineEdit* m_structDept;
    QLineEdit* m_structSemester;
    QDoubleSpinBox* m_structAmount;
    QLineEdit* m_structDesc;

    // student payments
    QComboBox* m_studentCombo;
    QTableWidget* m_paymentsTable;
    QDoubleSpinBox* m_paymentAmount;
    QLabel* m_balanceLabel;

    void loadStudents();
};
