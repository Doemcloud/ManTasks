#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>

class TaskManager : public QWidget {
    Q_OBJECT

public:
    TaskManager(QWidget *parent = nullptr);

private slots:
    void refreshProcesses();
    void endSelectedProcess();

private:
    QTableWidget *table;
    QPushButton *btnEndProcess;

    void setupUI();
};

#endif // TASKMANAGER_H
