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

#include "syntaxhighlighter.h"
#include "logparser.h"

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

private:
    void setupUI();
    void setupMenuBar();
    void setupStatusBar();
    void setupConnections();
    void populateTree();

    // виджеты
    QPlainTextEdit *m_logViewer;
    QTreeWidget    *m_treePanel;
    QLineEdit      *m_searchField;
    QSplitter      *m_splitter;
    QLabel         *m_statusLabel;

    // логика
    LogParser          m_parser;
    QVector<LogEntry>  m_entries;
    QString            m_currentFile;

    //подстветка синтаксиса
    SyntaxHighlighter *m_highlighter;

    QVector<int> m_searchResults;  // номера блоков с совпадениями
    int m_searchIndex;             // текущая позиция в результатах

    QPushButton *m_filterInfo;
    QPushButton *m_filterWarn;
    QPushButton *m_filterError;
    QPushButton *m_filterOther;

    void applyFilter();
    QString m_rawContent;  // оригинальный текст файла

    QDateTimeEdit *m_timeFrom;
    QDateTimeEdit *m_timeTo;
    QPushButton   *m_timeApply;
    QPushButton   *m_timeReset;
};

#endif // MAINWINDOW_H