#pragma once
#include <QMainWindow>

class QTabWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void logout();

private:
    void buildTabsForRole();
    QTabWidget* m_tabs;
};
