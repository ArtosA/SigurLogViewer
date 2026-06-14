#include "logparser.h"

LogParser::LogParser()
    : m_lastFormat(LogFormat::Unknown)
{
    // компилируем регулярки один раз в конструкторе — это быстрее
    // чем создавать их при каждом вызове parse

    // Quarkus: "2026-05-06 14:02:25,039  INFO PlatformDependent:-1 - message"
    m_quarkusRegex = QRegularExpression(
        R"((\d{4}-\d{2}-\d{2}\s+\d{2}:\d{2}:\d{2},\d{3})\s+(\w+)\s+(\S+)\s+-\s+(.*))"
        );
    //    ^timestamp(запятая перед мс)     ^level  ^source      ^message

    // Spring: "2026-05-06 12:31:52.221  WARN 1120 --- [thread-name] class : message"
    m_springRegex = QRegularExpression(
        R"((\d{4}-\d{2}-\d{2}\s+\d{2}:\d{2}:\d{2}\.\d{3})\s+(\w+)\s+(?:\d+\s+)?---\s+\[([^\]]+)\]\s+(\S+)\s*:\s+(.*))"
        );
    //    ^timestamp(точка перед мс)       ^level           ^thread    ^class    ^message

    // Semicolon: "2025-07-07 00:00:00; message"
    m_semicolonRegex = QRegularExpression(
        R"((\d{4}-\d{2}-\d{2}\s+\d{2}:\d{2}:\d{2});\s+(.*))"
        );
    //    ^timestamp(без мс)                          ^message
}

LogFormat LogParser::detectFormat(const QStringList& lines)
{
    // проверяем первые 10 строк — ищем совпадение с паттерном
    int limit = qMin(lines.size(), 10);

    for (int i = 0; i < limit; i++) {
        const QString& line = lines[i];

        if (m_quarkusRegex.match(line).hasMatch())
            return LogFormat::Quarkus;

        if (m_springRegex.match(line).hasMatch())
            return LogFormat::SpringBoot;

        if (m_semicolonRegex.match(line).hasMatch())
            return LogFormat::SimpleSemicolon;
    }
    return LogFormat::Unknown;
}

QVector<LogEntry> LogParser::parse(const QString& content)
{
    QVector<LogEntry> entries;
    QStringList lines = content.split('\n');

    // определяем формат
    m_lastFormat = detectFormat(lines);

    for (int i = 0; i < lines.size(); i++) {
        const QString& line = lines[i];

        if (line.trimmed().isEmpty())
            continue;

        // строки stacktrace присоединяем к предыдущей записи
        if (isStacktraceLine(line)) {
            if (!entries.isEmpty()) {
                entries.last().stacktrace.append(line);
            }
            continue;
        }

        // парсим строку в зависимости от формата
        LogEntry entry;
        switch (m_lastFormat) {
        case LogFormat::Quarkus:
            entry = parseQuarkusLine(line, i + 1);
            break;
        case LogFormat::SpringBoot:
            entry = parseSpringLine(line, i + 1);
            break;
        case LogFormat::SimpleSemicolon:
            entry = parseSemicolonLine(line, i + 1);
            break;
        default:
            // неизвестный формат — сохраняем как есть
            entry.lineNumber = i + 1;
            entry.rawText = line;
            entry.message = line;
            entry.level = LogLevel::Unknown;
            entry.format = LogFormat::Unknown;
            break;
        }

        entries.append(entry);
    }

    return entries;
}

LogEntry LogParser::parseQuarkusLine(const QString& line, int lineNum)
{
    LogEntry entry;
    entry.lineNumber = lineNum;
    entry.rawText = line;
    entry.format = LogFormat::Quarkus;

    auto match = m_quarkusRegex.match(line);
    if (match.hasMatch()) {
        // match.captured(1) — первая группа в скобках = timestamp
        entry.timestamp = QDateTime::fromString(
            match.captured(1), "yyyy-MM-dd HH:mm:ss,zzz"
            );
        entry.level   = LogEntry::stringToLevel(match.captured(2));
        entry.source  = match.captured(3);
        entry.message = match.captured(4);
    } else {
        // строка не совпала с паттерном — сохраняем как есть
        entry.message = line;
        entry.level = LogLevel::Unknown;
    }

    return entry;
}

LogEntry LogParser::parseSpringLine(const QString& line, int lineNum)
{
    LogEntry entry;
    entry.lineNumber = lineNum;
    entry.rawText = line;
    entry.format = LogFormat::SpringBoot;

    auto match = m_springRegex.match(line);
    if (match.hasMatch()) {
        entry.timestamp = QDateTime::fromString(
            match.captured(1), "yyyy-MM-dd HH:mm:ss.zzz"
            );
        entry.level   = LogEntry::stringToLevel(match.captured(2));
        entry.source  = match.captured(4);  // class name
        entry.message = match.captured(5);
    } else {
        entry.message = line;
        entry.level = LogLevel::Unknown;
    }

    return entry;
}

LogEntry LogParser::parseSemicolonLine(const QString& line, int lineNum)
{
    LogEntry entry;
    entry.lineNumber = lineNum;
    entry.rawText = line;
    entry.format = LogFormat::SimpleSemicolon;

    auto match = m_semicolonRegex.match(line);
    if (match.hasMatch()) {
        entry.timestamp = QDateTime::fromString(
            match.captured(1), "yyyy-MM-dd HH:mm:ss"
            );
        entry.level   = LogLevel::Info;  // dcwdt не имеет уровней — ставим Info
        entry.message = match.captured(2);
    } else {
        entry.message = line;
        entry.level = LogLevel::Unknown;
    }

    return entry;
}

bool LogParser::isStacktraceLine(const QString& line)
{
    if (line.isEmpty())
        return false;

    QString trimmed = line.trimmed();

    if (line.startsWith('\t'))
        return true;

    if (trimmed.startsWith("at "))
        return true;

    if (trimmed.startsWith("Caused by:"))
        return true;

    if (trimmed.startsWith("..."))
        return true;

    if (!line.isEmpty() && !line[0].isDigit())
        return true;

    return false;
}