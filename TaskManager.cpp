#include "TaskManager.h"
#include <windows.h>
#include <psapi.h>
#include <QVBoxLayout>
#include <QTimer>

TaskManager::TaskManager(QWidget *parent) : QWidget(parent) {
    setupUI();
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &TaskManager::refreshProcesses);
    timer->start(5000);  // Обновлять список каждые 5 секунд
}

void TaskManager::setupUI() {
    table = new QTableWidget(0, 4, this);
    table->setHorizontalHeaderLabels({"PID", "Имя файла", "Использование CPU", "Память (KB)"});
    btnEndProcess = new QPushButton("Завершить процесс", this);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(table);
    layout->addWidget(btnEndProcess);
    connect(btnEndProcess, &QPushButton::clicked, this, &TaskManager::endSelectedProcess);
}

void TaskManager::refreshProcesses() {
    DWORD processes[1024], processCount;
    if (!EnumProcesses(processes, sizeof(processes), &processCount)) {
        return;
    }
    processCount /= sizeof(DWORD);
    table->setRowCount(0);
    for (int i = 0; i < int(processCount); i++) {
        DWORD pid = processes[i];
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_TERMINATE, FALSE, pid);
        if (hProcess) {
            wchar_t processName[MAX_PATH] = L"<unknown>";
            HMODULE hMod;
            DWORD cbNeeded;
            if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded)) {
                GetModuleBaseName(hProcess, hMod, processName, sizeof(processName) / sizeof(wchar_t));
            }
            PROCESS_MEMORY_COUNTERS pmc;
            GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc));
            int row = table->rowCount();
            table->insertRow(row);
            table->setItem(row, 0, new QTableWidgetItem(QString::number(pid)));
            table->setItem(row, 1, new QTableWidgetItem(QString::fromWCharArray(processName)));
            table->setItem(row, 2, new QTableWidgetItem("<получение данных о CPU>"));
            table->setItem(row, 3, new QTableWidgetItem(QString::number(pmc.WorkingSetSize / 1024)));
            CloseHandle(hProcess);
        }
    }
}

void TaskManager::endSelectedProcess() {
    int row = table->currentRow();
    if (row != -1) {
        DWORD pid = table->item(row, 0)->text().toUInt();
        HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
        if (hProcess) {
            TerminateProcess(hProcess, 0);
            CloseHandle(hProcess);
            refreshProcesses();
        }
    }
}
