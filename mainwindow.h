#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPlainTextEdit>
#include <QTreeWidget>
#include <QLineEdit>
#include <QSplitter>
#include <QMenuBar>
#include <QStatusBar>
#include <QLabel>
#include <QShortcut>
#include <QPushButton>
#include <QDateTimeEdit>
#include <QProgressBar>
#include <QTabWidget>
#include <QDockWidget>
#include <QTextBrowser>

#include "logtab.h"
#include "syntaxhighlighter.h"
#include "logparser.h"
#include "fileloader.h"
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void openFile();
    void onTreeItemClicked(QTreeWidgetItem *item, int column);
    void onSearch();
    void onSearchNext();
    void onFilterChanged();
    void onTimeFilterApply();
    void onTimeFilterReset();
    void onLoadingProgress(int percent);
    void onLoadingFinished(const QString &content,
                           const QVector<LogEntry> &entries,
                           LogFormat format);
    void onLoadingFailed(const QString &error);
    void onTabChanged(int index);
    void onTabClosed(int index);
    void updateStatistics();

private:
    void setupUI();
    void setupMenuBar();
    void setupStatusBar();
    void setupConnections();
    void populateTree();
    void applyFilter();

    LogTab* currentTab();  // получить активную вкладку
    void setupStatsDock();

    // виджеты
    QTabWidget     *m_tabWidget;     // вкладки
    QTreeWidget    *m_treePanel;
    QLineEdit      *m_searchField;
    QSplitter      *m_splitter;
    QLabel         *m_statusLabel;

    QPushButton *m_filterInfo;
    QPushButton *m_filterWarn;
    QPushButton *m_filterError;
    QPushButton *m_filterOther;

    QDateTimeEdit *m_timeFrom;
    QDateTimeEdit *m_timeTo;
    QPushButton   *m_timeApply;
    QPushButton   *m_timeReset;

    FileLoader    *m_fileLoader;
    QProgressBar  *m_progressBar;

    // поиск
    QVector<int> m_searchResults;
    int m_searchIndex;

    // логика
    LogParser m_parser;

    //statistika
    QDockWidget   *m_statsDock;
    QTextBrowser  *m_statsView;
};

#endif // MAINWINDOW_H