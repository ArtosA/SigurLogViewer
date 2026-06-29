#ifndef UPDATECHECKER_H
#define UPDATECHECKER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QString>

class UpdateChecker : public QObject
{
    Q_OBJECT

public:
    explicit UpdateChecker(const QString &currentVersion, QObject *parent = nullptr);

    void checkForUpdates(bool silent = true);

signals:
    void updateAvailable(const QString &newVersion, const QString &downloadUrl, const QString &releaseNotes);
    void noUpdateAvailable();
    void checkFailed(const QString &error);

private slots:
    void onReplyFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager *m_manager;
    QString m_currentVersion;
    bool m_silent;
};

#endif // UPDATECHECKER_H