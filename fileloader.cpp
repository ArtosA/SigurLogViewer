#include "fileloader.h"
#include <QFile>
#include <QTextStream>

FileLoader::FileLoader(QObject *parent)
    : QThread(parent)
{
}

void FileLoader::loadFile(const QString &filePath)
{
    m_filePath = filePath;
    start();  // запускает run() в отдельном потоке
}

void FileLoader::run()
{
    // этот код выполняется НЕ в основном потоке
    // поэтому UI не замирает

    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit loadingFailed("Не удалось открыть файл: " + m_filePath);
        return;
    }

    emit progressChanged(10);

    // читаем файл
    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    emit progressChanged(40);

    // парсим
    LogParser parser;
    QVector<LogEntry> entries = parser.parse(content);

    emit progressChanged(90);

    // отправляем результат в основной поток
    emit loadingFinished(content, entries, parser.lastFormat());
}