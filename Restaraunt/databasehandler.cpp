#include "databasehandler.h"
#include <QNetworkRequest>
#include <QDebug>
databaseHandler::databaseHandler(QObject *parent) : QObject(parent) {
    m_networkManager = new QNetworkAccessManager (this);

    m_networkReply = m_networkManager->get(QNetworkRequest(QUrl("https://qtrestaraunt-default-rtdb.firebaseio.com/bookings.json")));
    connect(m_networkReply, &QNetworkReply::readyRead, this, &databaseHandler::networkReplyReadyRead);
}

databaseHandler::~databaseHandler() {
    m_networkManager->deleteLater();
}

void databaseHandler::networkReplyReadyRead() {
    qDebug() << m_networkReply->readAll();
}
