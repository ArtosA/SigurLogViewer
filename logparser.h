#ifndef LOGPARSER_H
#define LOGPARSER_H

#include "logentry.h"
#include <QVector>
#include <QString>
#include <QRegularExpression>

class LogParser {
public:
    LogParser();

    // главный метод — парсит текст, возвращает записи
    QVector<LogEntry> parse(const QString& content);

    // определить формат по первым строкам
    LogFormat detectFormat(const QStringList& lines);

    // геттер формата после парсинга
    LogFormat lastFormat() const { return m_lastFormat; }

private:
    LogEntry parseQuarkusLine(const QString& line, int lineNum);
    LogEntry parseSpringLine(const QString& line, int lineNum);
    LogEntry parseSemicolonLine(const QString& line, int lineNum);

    bool isStacktraceLine(const QString& line);

    // регулярные выражения — создаём один раз, используем многократно
    QRegularExpression m_quarkusRegex;
    QRegularExpression m_springRegex;
    QRegularExpression m_semicolonRegex;

    LogFormat m_lastFormat;
};

#endif // LOGPARSER_H