#ifndef FILELOADER_H
#define FILELOADER_H

#include <QThread>
#include <QString>
#include <QVector>
#include "logentry.h"
#include "logparser.h"

class FileLoader : public QThread
{
    Q_OBJECT

public:
    explicit FileLoader(QObject *parent = nullptr);

    void loadFile(const QString &filePath);

signals:
    // сигналы для обновления UI из основного потока
    void progressChanged(int percent);
    void loadingFinished(const QString &content,
                         const QVector<LogEntry> &entries,
                         LogFormat format);
    void loadingFailed(const QString &error);

protected:
    // этот метод выполняется в отдельном потоке
    void run() override;

private:
    QString m_filePath;
};

#endif // FILELOADER_H