#ifndef LOGENTRY_H
#define LOGENTRY_H

#include <QString>
#include <QDateTime>
#include <QStringList>

// уровни логирования
enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error,
    Fatal,
    Unknown
};

// форматы логов
enum class LogFormat {
    Quarkus,          // local-agent: "2026-05-06 14:02:25,039  INFO Source:-1 - msg"
    SpringBoot,       // notification: "2026-05-06 12:31:52.221  WARN 1120 --- [thread] class : msg"
    SimpleSemicolon,  // dcwdt: "2025-07-07 00:00:00; msg"
    Unknown
};

// одна запись лога
struct LogEntry {
    int lineNumber;            // номер строки в файле
    QDateTime timestamp;       // временная метка
    LogLevel level;            // уровень
    QString source;            // источник (класс/компонент)
    QString message;           // сообщение
    QStringList stacktrace;    // стек вызовов
    LogFormat format;          // формат
    QString rawText;           // оригинальная строка

    // вспомогательные функции
    static QString levelToString(LogLevel lvl) {
        switch (lvl) {
        case LogLevel::Debug:   return "DEBUG";
        case LogLevel::Info:    return "INFO";
        case LogLevel::Warning: return "WARNING";
        case LogLevel::Error:   return "ERROR";
        case LogLevel::Fatal:   return "FATAL";
        default:                return "UNKNOWN";
        }
    }

    static LogLevel stringToLevel(const QString& str) {
        QString upper = str.trimmed().toUpper();
        if (upper == "DEBUG")                    return LogLevel::Debug;
        if (upper == "INFO")                     return LogLevel::Info;
        if (upper == "WARN" || upper == "WARNING") return LogLevel::Warning;
        if (upper == "ERROR")                    return LogLevel::Error;
        if (upper == "FATAL")                    return LogLevel::Fatal;
        return LogLevel::Unknown;
    }
};

#endif // LOGENTRY_H