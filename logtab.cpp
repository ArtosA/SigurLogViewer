#include "logtab.h"
#include <QVBoxLayout>

LogTab::LogTab(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    logViewer = new QPlainTextEdit(this);
    logViewer->setReadOnly(true);
    logViewer->setLineWrapMode(QPlainTextEdit::NoWrap);
    logViewer->setFont(QFont("Consolas", 10));
    logViewer->setPlaceholderText("Открой файл: Файл → Открыть (Ctrl+O)");

    highlighter = new SyntaxHighlighter(logViewer->document());

    layout->addWidget(logViewer);
}