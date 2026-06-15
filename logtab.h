#ifndef LOGTAB_H
#define LOGTAB_H

#include <QWidget>
#include <QPlainTextEdit>
#include <QVector>
#include <QString>
#include "logentry.h"
#include "syntaxhighlighter.h"

class LogTab : public QWidget
{
    Q_OBJECT

public:
    explicit LogTab(QWidget *parent = nullptr);

    QPlainTextEdit    *logViewer;
    SyntaxHighlighter *highlighter;
    QVector<LogEntry>  entries;
    QString            rawContent;
    QString            filePath;
    LogFormat          format;
};

#endif // LOGTAB_H