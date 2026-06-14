#include "syntaxhighlighter.h"

SyntaxHighlighter::SyntaxHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    // ERROR — красный
    HighlightRule errorRule;
    errorRule.pattern = QRegularExpression(R"(.*\bERROR\b.*)");
    errorRule.format.setForeground(QColor("#FF6B6B"));
    errorRule.format.setFontWeight(QFont::Bold);
    m_rules.append(errorRule);

    // WARN/WARNING — жёлтый
    HighlightRule warnRule;
    warnRule.pattern = QRegularExpression(R"(.*\bWARN(?:ING)?\b.*)");
    warnRule.format.setForeground(QColor("#FFD93D"));
    m_rules.append(warnRule);

    // INFO — зелёный
    HighlightRule infoRule;
    infoRule.pattern = QRegularExpression(R"(.*\bINFO\b.*)");
    infoRule.format.setForeground(QColor("#6BCB77"));
    m_rules.append(infoRule);

    // DEBUG — серый
    HighlightRule debugRule;
    debugRule.pattern = QRegularExpression(R"(.*\bDEBUG\b.*)");
    debugRule.format.setForeground(QColor("#888888"));
    m_rules.append(debugRule);

    // stacktrace (строки начинающиеся с tab или "at ") — приглушённый
    HighlightRule stackRule;
    stackRule.pattern = QRegularExpression(R"(^[\t\s]+(at |Caused by:|\.\.\.).*)");
    stackRule.format.setForeground(QColor("#777777"));
    stackRule.format.setFontItalic(true);
    m_rules.append(stackRule);

    // timestamp — голубой
    HighlightRule timeRule;
    timeRule.pattern = QRegularExpression(R"(\d{4}-\d{2}-\d{2}\s+\d{2}:\d{2}:\d{2}[.,]?\d{0,3})");
    timeRule.format.setForeground(QColor("#4FC3F7"));
    m_rules.append(timeRule);
}

void SyntaxHighlighter::highlightBlock(const QString &text)
{
    // проверяем каждое правило
    for (const auto &rule : m_rules) {
        auto match = rule.pattern.match(text);
        if (match.hasMatch()) {
            // подсвечиваем весь матч
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}