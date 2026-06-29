#include "updatechecker.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QVersionNumber>

UpdateChecker::UpdateChecker(const QString &currentVersion, QObject *parent)
    : QObject(parent)
    , m_currentVersion(currentVersion)
    , m_silent(true)
{
    m_manager = new QNetworkAccessManager(this);
    connect(m_manager, &QNetworkAccessManager::finished,
            this, &UpdateChecker::onReplyFinished);
}

void UpdateChecker::checkForUpdates(bool silent)
{
    m_silent = silent;

    // GitHub API — последний релиз
    QNetworkRequest request(
        QUrl("https://api.github.com/repos/ArtosA/SigurLogViewer/releases/latest")
        );
    request.setHeader(QNetworkRequest::UserAgentHeader, "SigurLogViewer");
    request.setRawHeader("Accept", "application/vnd.github.v3+json");

    m_manager->get(request);
}

void UpdateChecker::onReplyFinished(QNetworkReply *reply)
{
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        if (!m_silent)
            emit checkFailed(reply->errorString());
        return;
    }

    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);

    if (!doc.isObject()) {
        if (!m_silent)
            emit checkFailed("Неверный ответ от GitHub");
        return;
    }

    QJsonObject obj = doc.object();
    QString tagName = obj["tag_name"].toString();      // "v0.6.0"
    QString body = obj["body"].toString();              // release notes
    QString htmlUrl = obj["html_url"].toString();       // ссылка на релиз

    // убираем "v" из тега
    QString remoteVersion = tagName;
    if (remoteVersion.startsWith("v"))
        remoteVersion = remoteVersion.mid(1);

    QString localVersion = m_currentVersion;
    if (localVersion.startsWith("v"))
        localVersion = localVersion.mid(1);

    // сравниваем версии
    QVersionNumber remote = QVersionNumber::fromString(remoteVersion);
    QVersionNumber local = QVersionNumber::fromString(localVersion);

    if (remote > local) {
        emit updateAvailable(tagName, htmlUrl, body);
    } else {
        if (!m_silent)
            emit noUpdateAvailable();
    }
}