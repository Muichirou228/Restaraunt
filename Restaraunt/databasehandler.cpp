#include "databasehandler.h"
#include <QDebug>
#include <QUrl>
#include <QNetworkRequest>
#include <QJsonObject>
#include <QJsonArray>

databaseHandler::databaseHandler(QObject *parent) : QObject(parent), m_networkManager(new QNetworkAccessManager(this)) {
    qDebug() << "Databasehandler initialized";
}

void databaseHandler::fetchBookingsForDate(const QString &date) {
    if (date.isEmpty()) return;

    qDebug() << "Requesting bookings for date: " << date;
    QString url = QString ("https://qtrestaraunt-default-rtdb.firebaseio.com/"
                          "bookings.json?orderBy=\"date\"&equalTo=\"%1\"").arg(date);
    QNetworkReply *reply = m_networkManager->get(QNetworkRequest(QUrl(url)));
    connect(reply, &QNetworkReply::finished, this, [this,reply]() {
        handleNetworkReply(reply);
        reply->deleteLater();
    });
}

void databaseHandler::handleNetworkReply(QNetworkReply *reply) {
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "Network error:" << reply->errorString();
        return;
    }

    // Читаем и парсим JSON-ответ
    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);

    if (!doc.isObject()) {
        qDebug() << "Invalid JSON format";
        return;
    }

    // Очищаем предыдущие данные
    m_bookedTables.clear();

    // Извлекаем номера столов из бронирований
    QJsonObject bookings = doc.object();
    for (auto it = bookings.begin(); it != bookings.end(); ++it) {
        QJsonObject booking = it.value().toObject();
        if (booking.contains("table_num")) {
            m_bookedTables.append(booking["table_num"].toInt());
        }
    }

    qDebug() << "Fetched booked tables:" << m_bookedTables;

    // Отправляем сигнал с обновленными данными
    emit bookingsFetched(m_bookedTables);
}


