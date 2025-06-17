#include "databasehandler.h"
#include <QDebug>
#include <QUrl>
#include <QNetworkRequest>
#include <QJsonObject>
#include <QJsonArray>

databaseHandler::databaseHandler(QObject *parent) : QObject(parent), m_networkManager(new QNetworkAccessManager(this)) {
    qDebug() << "Databasehandler initialized";
}

void databaseHandler::getBookingsOnDate(const QString &date) {
    if (date.isEmpty()) return;

    qDebug() << "Requesting bookings for date: " << date;
    QString url = QString ("https://qtrestaraunt-default-rtdb.firebaseio.com/"
                          "bookings.json?orderBy=\"date\"&equalTo=\"%1\"").arg(date);
    QNetworkReply *reply = m_networkManager->get(QNetworkRequest(QUrl(url)));
    connect(reply, &QNetworkReply::finished, this, [this,reply]() {
        handleNetworkReply(reply);
    });
}

void databaseHandler::clearBookings() {
    m_bookedTables.clear();
}

void databaseHandler::handleNetworkReply(QNetworkReply *reply) {
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "Network error:" << reply->errorString();
        reply->deleteLater();
        return;
    }

    // Читаем и парсим JSON-ответ
    QByteArray data = reply->readAll();
    reply->deleteLater();

    qDebug() << "Raw response: " << data;

    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "JSON error: " << parseError.errorString();
        return;
    }

    if (!jsonDoc.isObject()) {
        qWarning() << "Expected JSON object in response";
        return;
    }

    QJsonObject rootObject = jsonDoc.object();
    QList<int> bookedTablesList;

    for (auto it = rootObject.begin(); it != rootObject.end(); ++it) {
        QJsonValue bookingValue = it.value();
        if (!bookingValue.isObject()) {
            qDebug() << "Skipping non-object booking";
            continue;
        }
        QJsonObject booking = bookingValue.toObject();

        int tableNumber = booking["table_num"].toInt();
        qDebug() << "Found table on this date with number " << tableNumber;
        bookedTablesList.append(tableNumber);
    }
    qDebug() << "Total count booked tables:" << bookedTablesList.size();
    m_bookedTables = bookedTablesList;
    emit bookingsReady(bookedTablesList);
}

QList<int> databaseHandler::bookedTables() const {
    return m_bookedTables;
}


