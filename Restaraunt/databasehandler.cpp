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

void databaseHandler::checkIfClientExists(QString firstName, QString secondName, QString thirdName, int age) {
    QString foundId = "";

    QNetworkRequest request(QUrl("https://qtrestaraunt-default-rtdb.firebaseio.com/clients.json"));
    QNetworkReply *reply = m_networkManager->get(request);

    connect(reply, &QNetworkReply::finished, [=]() mutable {
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            QJsonObject clients = doc.object();

            for (const QString &clientId : clients.keys()) {
                QJsonObject client = clients.value(clientId).toObject();
                if (client["name"].toString().compare(firstName, Qt::CaseInsensitive) == 0 &&
                    client["second_name"].toString().compare(secondName, Qt::CaseInsensitive) == 0 &&
                    client["third_name"].toString().compare(thirdName, Qt::CaseInsensitive) == 0 &&
                    client["age"] == age) {
                    foundId = clientId;
                    break;
                }
            }
        }
        qDebug() << "Сигнал сработал clientChecked";
        emit clientChecked(foundId);
        reply->deleteLater();
    });
}

void databaseHandler::addNewClient (QString firstName, QString secondName, QString thirdName, int age) {
    QJsonObject newClient;
    newClient["name"] = firstName;
    newClient["second_name"] = secondName;
    newClient["third_name"] = thirdName;
    newClient["age"] = age;

    QNetworkRequest request(QUrl("https://qtrestaraunt-default-rtdb.firebaseio.com/clients.json"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QNetworkReply *reply = m_networkManager->post(request, QJsonDocument(newClient).toJson());

    connect(reply, &QNetworkReply::finished, [=]() {
        if (reply->error() != QNetworkReply::NoError) {
            qWarning() << "Ошибка при добавлении клиента:" << reply->errorString();
            emit clientAdded("");
            reply->deleteLater();
            return;
        }

        QJsonDocument response = QJsonDocument::fromJson(reply->readAll());
        QString clientId = response.object().value("name").toString();
        if (!clientId.isEmpty()) {
            qDebug() << "Клиент успешно добавлен с ID:" << clientId;
            emit clientAdded(clientId);
        } else {
            emit clientAdded("");
        }
        reply->deleteLater();
    });
}

void databaseHandler::addNewBooking(const QString date, const QList<int> tables, const QString id_client)
{
    // Проверка входных данных
    if (date.isEmpty() || id_client.isEmpty()) {
        emit bookingAdded(false, "Неверные параметры");
        return;
    }

    // 1. Сначала проверяем существующие бронирования
    QString path = QString("bookings.json?orderBy=\"id_client\"&equalTo=\"%1\"")
                       .arg(id_client);
    QUrl checkUrl("https://qtrestaraunt-default-rtdb.firebaseio.com/" + path);

    QNetworkRequest checkRequest(checkUrl);
    checkRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *checkReply = m_networkManager->get(checkRequest);

    connect(checkReply, &QNetworkReply::finished, [=]() {
        if (checkReply->error() != QNetworkReply::NoError) {
            QString errorMsg = QString("Ошибка проверки бронирований: %1")
                                   .arg(checkReply->errorString());
            emit bookingAdded(false, errorMsg);
            checkReply->deleteLater();
            return;
        }

        // Анализ существующих бронирований
        QJsonDocument doc = QJsonDocument::fromJson(checkReply->readAll());
        QJsonObject bookings = doc.object();
        int existingCount = bookings.keys().size();

        // Проверка лимита (максимум 2 бронирования)
        if (existingCount + tables.size() > 2) {
            emit bookingAdded(false, "Превышен лимит бронирований (максимум 2)");
            checkReply->deleteLater();
            return;
        }

        // 2. Добавление новых бронирований
        for (int tableNum : tables) {
            QJsonObject booking;
            booking["date"] = date;
            booking["id_client"] = id_client;
            booking["table_num"] = tableNum;

            QUrl addUrl("https://qtrestaraunt-default-rtdb.firebaseio.com/bookings.json");
            QNetworkRequest addRequest(addUrl);
            addRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

            QNetworkReply *addReply = m_networkManager->post(
                addRequest,
                QJsonDocument(booking).toJson()
                );

            connect(addReply, &QNetworkReply::finished, [=]() {
                bool success = addReply->error() == QNetworkReply::NoError;
                QString message = success ? "Бронирование добавлено"
                                          : "Ошибка: " + addReply->errorString();
                emit bookingAdded(success, message);
                addReply->deleteLater();
            });
        }

        checkReply->deleteLater();
    });
}

QList<int> databaseHandler::convertToListFromString(QString text) {
    QList<int> result;
    QStringList parts = text.split(' ', Qt::SkipEmptyParts);

    for (const QString &part : parts) {
        bool ok;
        int num = part.toInt(&ok);
        if (ok) {
            result.append(num);
        }
    }

    return result;
}

void databaseHandler::checkIfUserCodeExists(QString code) {
    if (code.isEmpty()) {
        qDebug() << "Empty waiter code provided";
        emit codeChecked(""); // Возвращаем пустую строку
        return;
    }

    // Формируем URL запроса
    QUrl url(QString("https://qtrestaraunt-default-rtdb.firebaseio.com/waiters.json?orderBy=\"code\"&equalTo=\"%1\"")
                 .arg(QString(QUrl::toPercentEncoding(code))));

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // Отправляем асинхронный запрос
    QNetworkReply *reply = m_networkManager->get(request);

    // Обрабатываем ответ
    connect(reply, &QNetworkReply::finished, [this, reply, code]() {
        QString result = "";
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        QJsonObject waiters = doc.object();
        if (reply->error() == QNetworkReply::NoError) {

            // Если есть хотя бы один официант с таким кодом
            if (!waiters.isEmpty()) {
                qDebug() << "Valid waiter code found:" << code;
            }
        } else {
            qDebug() << "Error checking waiter code:" << reply->errorString();
        }

        for (const QString &key : waiters.keys()) {
            QJsonObject waiter = waiters.value(key).toObject();
            if (waiter.contains("name")) {
                result = waiter["name"].toString();
                qDebug() << "Found waiter with name " << result;
                break; // Берем первого найденного
            }
        }

        emit codeChecked(result); // Отправляем результат
        reply->deleteLater(); // Удаляем reply объект
    });
}

QString databaseHandler::checkTableStatus(QString table_num) {
    QUrl url(QString("https://qtrestaraunt-default-rtdb.firebaseio.com/tables.json?orderBy=\"table_num\"&equalTo=\"%1\"")
                 .arg(QString(QUrl::toPercentEncoding(table_num))));

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = m_networkManager->get(request);

    connect(reply, &QNetworkReply::finished, [this, reply, table_num]() {
        QString result = "";
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        QJsonObject waiters = doc.object();
        if (reply->error() == QNetworkReply::NoError) {

            // Если есть хотя бы один официант с таким кодом
            if (!waiters.isEmpty()) {
                qDebug() << "Valid table info found for number " << table_num;
            }
        }

        for (const QString &key : waiters.keys()) {
            QJsonObject waiter = waiters.value(key).toObject();
            if (waiter.contains("status")) {
                result = waiter["status"].toString();
                qDebug() << "Found table number " << table_num << "with status " << result;
                break;
            }
        }

        tableStatusChecked(table_num, result); // Отправляем результат
        reply->deleteLater(); // Удаляем reply объект
    });
}




