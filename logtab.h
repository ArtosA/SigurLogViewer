#ifndef LOGTAB_H
#define LOGTAB_H

#include <QWidget>
#include <QVector>
#include <QString>
#include "logentry.h"
#include "syntaxhighlighter.h"
#include "linenumberarea.h"

class LogTab : public QWidget
{
    Q_OBJECT

public:
    explicit LogTab(QWidget *parent = nullptr);

    CodeEditor        *logViewer;     // было QPlainTextEdit
    SyntaxHighlighter *highlighter;
    QVector<LogEntry>  entries;
    QString            rawContent;
    QString            filePath;
    LogFormat          format;
};

#endif // LOGTAB_H