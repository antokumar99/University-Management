#pragma once
#include <QWidget>

class QComboBox;
class QLineEdit;
class QDateEdit;

class ReportsWidget : public QWidget {
    Q_OBJECT
public:
    explicit ReportsWidget(QWidget* parent = nullptr);

private slots:
    void exportTranscript();
    void exportAttendance();
    void exportFeeStatement();

private:
    QComboBox* m_transcriptStudent;
    QComboBox* m_feeStudent;
    QComboBox* m_attCourse;
    QLineEdit* m_attSemester;
    QDateEdit* m_attDate;

    void loadStudentCombos();
    void loadCourseCombo();
    QString pickSavePath(const QString& suggestedName);
};
