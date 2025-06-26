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
                //qDebug() << "Valid table info found for number " << table_num;
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

void databaseHandler::getTableOrders(int tableNumber)
{
    // 1. Находим активный заказ для указанного стола из таблицы tables
    QUrl tablesUrl(QString("https://qtrestaraunt-default-rtdb.firebaseio.com/tables.json?orderBy=\"table_num\"&equalTo=\"%1\"").arg(tableNumber));

    QNetworkReply *tablesReply = m_networkManager->get(QNetworkRequest(tablesUrl));

    connect(tablesReply, &QNetworkReply::finished, [this, tablesReply]() {
        if (tablesReply->error() == QNetworkReply::NoError) {
            QJsonDocument doc = QJsonDocument::fromJson(tablesReply->readAll());
            QJsonObject tablesData = doc.object();

            // Ищем таблицу с нужным номером
            for (const QString &tableKey : tablesData.keys()) {
                QJsonObject table = tablesData[tableKey].toObject();
                if (table.contains("order_num")) {
                    QString orderNum = table["order_num"].toString();
                    qDebug() << "Found order num for table " << "and its " << orderNum;
                    // 2. Проверяем статус заказа в orders
                    QUrl ordersUrl(QString("https://qtrestaraunt-default-rtdb.firebaseio.com/orders/%1.json").arg(orderNum));
                    QNetworkReply *ordersReply = m_networkManager->get(QNetworkRequest(ordersUrl));

                    connect(ordersReply, &QNetworkReply::finished, [this, ordersReply, orderNum]() {
                        if (ordersReply->error() == QNetworkReply::NoError) {
                            QJsonDocument orderDoc = QJsonDocument::fromJson(ordersReply->readAll());
                            QJsonObject orderData = orderDoc.object();

                            // Проверяем, что заказ активен (не завершен)
                            if (orderData["status"].toString() != "completed") {
                                // 3. Получаем позиции заказа из order_items
                                qDebug() << "FETORDERITEMS";
                                fetchOrderItems(orderNum);
                            }
                        }
                        ordersReply->deleteLater();
                    });
                    break;
                }
            }
        } else {
            emit errorOccurred("Failed to fetch table data: " + tablesReply->errorString());
        }
        tablesReply->deleteLater();
    });
}

void databaseHandler::fetchOrderItems(const QString &orderId)
{
    // Получаем все items для конкретного заказа
    QUrl orderItemsUrl(QString("https://qtrestaraunt-default-rtdb.firebaseio.com/order-items/%1.json").arg(orderId));

    QNetworkReply *orderItemsReply = m_networkManager->get(QNetworkRequest(orderItemsUrl));

    connect(orderItemsReply, &QNetworkReply::finished, [this, orderItemsReply]() {
        if (orderItemsReply->error() == QNetworkReply::NoError) {
            QJsonDocument doc = QJsonDocument::fromJson(orderItemsReply->readAll());
            QJsonObject orderItems = doc.object();

            QVariantList productRequests;

            // Перебираем все items в заказе
            for (const QString &itemKey : orderItems.keys()) {
                QJsonObject item = orderItems[itemKey].toObject();

                // Проверяем, что это item с продуктом
                if (item.contains("id_product")) {
                    QString productId = item["id_product"].toString();
                    int quantity = item["product_count"].toInt(1);

                    // Для отладки выведем в консоль
                    qDebug() << "Found item:" << productId << "Quantity:" << quantity;

                    QVariantMap productRequest;
                    productRequest["productId"] = productId;
                    productRequest["quantity"] = quantity;
                    productRequests.append(productRequest);
                }
            }

            if (!productRequests.isEmpty()) {
                fetchProductsInfo(productRequests);
            } else {
                qDebug() << "No items found in order";
                emit tableOrdersReady(QVariantList());
            }
        } else {
            qDebug() << "Error fetching order items:" << orderItemsReply->errorString();
            emit errorOccurred("Failed to fetch order items: " + orderItemsReply->errorString());
        }
        orderItemsReply->deleteLater();
    });
}

void databaseHandler::fetchProductsInfo(const QVariantList &productRequests)
{
    // Формируем один запрос для всех продуктов
    QUrl productsUrl("https://qtrestaraunt-default-rtdb.firebaseio.com/products.json");

    QNetworkReply *productsReply = m_networkManager->get(QNetworkRequest(productsUrl));

    connect(productsReply, &QNetworkReply::finished, [this, productsReply, productRequests]() {
        if (productsReply->error() == QNetworkReply::NoError) {
            QJsonDocument doc = QJsonDocument::fromJson(productsReply->readAll());
            QJsonObject allProducts = doc.object();

            QVariantList orderItems;

            // Сопоставляем запрошенные продукты с их данными
            for (const QVariant &request : productRequests) {
                QVariantMap req = request.toMap();
                QString productId = req["productId"].toString();
                int quantity = req["quantity"].toInt();

                if (allProducts.contains(productId)) {
                    QJsonObject product = allProducts[productId].toObject();

                    QVariantMap orderItem;
                    orderItem["name"] = product["product_name"].toString(); // Из вашей структуры
                    orderItem["price"] = product["price"].toDouble();      // Из вашей структуры
                    orderItem["quantity"] = quantity;
                    orderItem["total"] = orderItem["price"].toDouble() * quantity;
                    qDebug() << "ALL INFO ABOUT PRODUCTS : " << orderItem["name"] << orderItem["price"] << orderItem["quantity"] << orderItem["total"];
                    orderItems.append(orderItem);
                }
            }

            emit tableOrdersReady(orderItems);
        } else {
            emit errorOccurred("Failed to fetch products: " + productsReply->errorString());
        }
        productsReply->deleteLater();
    });
}

void databaseHandler::loadProducts() {
    QUrl productsUrl("https://qtrestaraunt-default-rtdb.firebaseio.com/products.json");
    QNetworkReply *reply = m_networkManager->get(QNetworkRequest(productsUrl));

    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray response = reply->readAll();
            QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
            QJsonObject productsObj = jsonDoc.object();

            m_productList.clear();
            //QStringList result;
            for (auto it = productsObj.begin(); it != productsObj.end(); ++it) {
                QJsonObject product = it.value().toObject();
                if (product.contains("product_name")) {
                    m_productList.append(product["product_name"].toString());
                    qDebug() << "Found product with name " << product["product_name"].toString();
                }
            }
            emit productsLoaded();
        } else {
            emit errorOccurred(reply->errorString());
        }
        reply->deleteLater();
    });
}

void databaseHandler::checkIfOrderExists(int table_num, const QVariantList &items) {
    QNetworkRequest request(QUrl("https://qtrestaraunt-default-rtdb.firebaseio.com/tables.json"));
    QNetworkReply *reply = m_networkManager->get(request);

    connect(reply, &QNetworkReply::finished, [=]() {
        if (reply->error() != QNetworkReply::NoError) {
            emit errorOccurred("Ошибка сети: " + reply->errorString());
            reply->deleteLater();
            return;
        }
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        QJsonObject tables = doc.object();
        bool orderExisting = true;
        QString tableNUM = QString::number(table_num);
        for (const QString &tableKey : tables.keys()) {
            QJsonObject table = tables.value(tableKey).toObject();

            if (table["table_num"].toString() == tableNUM) {
                if (table["id_waiter"] == "NULL" &&
                    table["order_num"] == "NULL") {
                    orderExisting = false;
                }
                break;
            }
        }
        qDebug() << "OrderExisting is " << orderExisting;
        emit orderExistingChecked(orderExisting, items);
        reply->deleteLater();
    });
}

void databaseHandler::findWaiterIdByName(const QString &waiterName, int table_num, const QVariantList &items)
{
    QNetworkRequest request(QUrl("https://qtrestaraunt-default-rtdb.firebaseio.com/waiters.json"));
    QNetworkReply *reply = m_networkManager->get(request);

    connect(reply, &QNetworkReply::finished, [=]() {
        QString foundId;
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            QJsonObject waiters = doc.object();

            for (const QString &waiterId : waiters.keys()) {
                QJsonObject waiter = waiters.value(waiterId).toObject();
                if (waiter["name"].toString().compare(waiterName, Qt::CaseInsensitive) == 0) {
                    foundId = waiterId;
                    qDebug() << "Found waiter with id " << foundId;
                    break;
                }
            }
        }
        addOrderInTable(table_num, foundId, items);
        reply->deleteLater();
    });
}

void databaseHandler::addOrderInTable(int table_num, QString id_waiter, const QVariantList &items)
{
    // Проверка инициализации
    if (!m_networkManager) {
        emit errorOccurred("Network manager not initialized");
        return;
    }

    // Проверка входных параметров
    if (table_num <= 0) {
        emit errorOccurred("Invalid table number");
        return;
    }

    if (id_waiter.isEmpty()) {
        emit errorOccurred("Waiter ID is empty");
        return;
    }

    // Преобразуем номер стола в строку
    QString tableNumStr = QString::number(table_num);

    // Генерация ID нового заказа
    const QString newOrderId = generateOrderId();

    // Создание объекта заказа
    QJsonObject newOrder;
    newOrder["status"] = "cooking";

    // Базовый URL Firebase
    const QString baseUrl = "https://qtrestaraunt-default-rtdb.firebaseio.com/";

    // 1. Поиск стола по номеру (как строке)
    QUrl findTableUrl(baseUrl + "tables.json");
    QString query = QString("orderBy=\"table_num\"&equalTo=\"%1\"").arg(tableNumStr);
    findTableUrl.setQuery(query);

    QNetworkRequest findRequest(findTableUrl);
    findRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *findReply = m_networkManager->get(findRequest);
    findReply->setParent(this);

    connect(findReply, &QNetworkReply::finished, this, [=]() {
        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> replyGuard(findReply);

        if (findReply->error() != QNetworkReply::NoError) {
            emit errorOccurred("Table search error: " + findReply->errorString());
            return;
        }

        // Парсинг ответа
        QJsonParseError parseError;
        QJsonDocument tableDoc = QJsonDocument::fromJson(findReply->readAll(), &parseError);

        if (parseError.error != QJsonParseError::NoError) {
            emit errorOccurred("JSON parse error: " + parseError.errorString());
            return;
        }

        QJsonObject tables = tableDoc.object();
        QString tableKey;

        // Поиск ключа таблицы (сравниваем как строки)
        for (const QString &key : tables.keys()) {
            QJsonObject table = tables.value(key).toObject();
            qDebug() << "Checking table:" << table["table_num"].toString();

            if (table["table_num"].toString() == tableNumStr) {
                tableKey = key;
                break;
            }
        }

        if (tableKey.isEmpty()) {
            emit errorOccurred(QString("Table %1 not found").arg(tableNumStr));
            qDebug() << "Available tables data:" << tables;
            return;
        }

        // 2. Создание нового заказа
        QNetworkRequest orderRequest(QUrl(baseUrl + "orders/" + newOrderId + ".json"));
        orderRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        QNetworkReply *orderReply = m_networkManager->put(orderRequest, QJsonDocument(newOrder).toJson());
        orderReply->setParent(this);

        connect(orderReply, &QNetworkReply::finished, this, [=]() {
            QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> orderGuard(orderReply);

            if (orderReply->error() != QNetworkReply::NoError) {
                emit errorOccurred("Order creation error: " + orderReply->errorString());
                return;
            }

            // 3. Обновление информации о столе
            QJsonObject tableUpdate;
            tableUpdate["order_num"] = newOrderId;
            tableUpdate["id_waiter"] = id_waiter;
            tableUpdate["status"] = "occupied";
            tableUpdate["id_booking"] = "NULL";
            tableUpdate["table_num"] = tableNumStr; // Сохраняем как строку

            QNetworkRequest tableRequest(QUrl(baseUrl + "tables/" + tableKey + ".json"));
            tableRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

            QNetworkReply *tableReply = m_networkManager->put(
                tableRequest,
                QJsonDocument(tableUpdate).toJson()
                );
            tableReply->setParent(this);

            connect(tableReply, &QNetworkReply::finished, this, [=]() {
                QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> tableGuard(tableReply);

                if (tableReply->error() != QNetworkReply::NoError) {
                    emit errorOccurred("Table update error: " + tableReply->errorString());
                } else {
                    // 4. Создание записи в order_items
                    QNetworkRequest orderItemsRequest(QUrl(baseUrl + "order-items/" + newOrderId + ".json"));
                    orderItemsRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

                    // Создаем пустой объект для нового заказа
                    QJsonObject newOrderItem;
                    newOrderItem["status"] = "created";

                    QNetworkReply *orderItemsReply = m_networkManager->put(
                        orderItemsRequest,
                        QJsonDocument(newOrderItem).toJson()
                        );
                    orderItemsReply->setParent(this);

                    connect(orderItemsReply, &QNetworkReply::finished, this, [=]() {
                        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> itemsGuard(orderItemsReply);

                        if (orderItemsReply->error() != QNetworkReply::NoError) {
                            emit errorOccurred("Order items creation error: " + orderItemsReply->errorString());
                        } else {
                            emit newOrderAddedInTable(table_num);
                            qDebug() << "Successfully created order" << newOrderId << "in order-items";
                            addOrderItems(newOrderId, items);
                        }
                    });
                }
            });
        });
    });
}

QString databaseHandler::generateOrderId() const
{
    return "order_" + QString::number(QDateTime::currentMSecsSinceEpoch());
}


void databaseHandler::addOrderItems(const QString &orderId, const QVariantList &items)
{
    if (!m_networkManager) {
        qWarning() << "Network manager not initialized";
        //emit orderItemsAdded();
        return;
    }

    if (orderId.isEmpty() || items.isEmpty()) {
        qWarning() << "Invalid input parameters";
        //emit orderItemsAdded();
        return;
    }

    // 1. Сначала получаем все продукты
    QNetworkRequest productsRequest(QUrl("https://qtrestaraunt-default-rtdb.firebaseio.com/products.json"));
    QNetworkReply *productsReply = m_networkManager->get(productsRequest);

    connect(productsReply, &QNetworkReply::finished, this, [=]() {
        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> productsGuard(productsReply);

        if (productsReply->error() != QNetworkReply::NoError) {
            qWarning() << "Failed to load products:" << productsReply->errorString();
            //emit orderItemsAdded();
            return;
        }

        QJsonParseError parseError;
        QJsonDocument productsDoc = QJsonDocument::fromJson(productsReply->readAll(), &parseError);
        if (parseError.error != QJsonParseError::NoError) {
            qWarning() << "Failed to parse products:" << parseError.errorString();
            //emit orderItemsAdded();
            return;
        }

        QJsonObject products = productsDoc.object();
        QMap<QString, QString> productNameToIdMap;

        // Создаем карту соответствия имени продукта к его ID
        for (const QString &productId : products.keys()) {
            QJsonObject product = products[productId].toObject();
            QString productName = product["product_name"].toString();
            productNameToIdMap[productName] = productId;
        }

        // 2. Получаем текущие items заказа, чтобы определить следующий индекс
        QNetworkRequest orderItemsRequest(QUrl(
            QString("https://qtrestaraunt-default-rtdb.firebaseio.com/order-items/%1.json").arg(orderId)));
        QNetworkReply *orderItemsReply = m_networkManager->get(orderItemsRequest);

        connect(orderItemsReply, &QNetworkReply::finished, this, [=]() mutable {
            QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> itemsGuard(orderItemsReply);

            int baseIndex = 0;
            if (orderItemsReply->error() == QNetworkReply::NoError) {
                QJsonDocument doc = QJsonDocument::fromJson(orderItemsReply->readAll());
                if (doc.isObject()) {
                    baseIndex = doc.object().count();
                }
            }

            // 3. Добавляем каждый item с правильным product_id
            int addedCount = 0;
            for (int i = 0; i < items.count(); ++i) {
                QVariantMap item = items[i].toMap();
                QString productName = item["name"].toString();
                QString productId = productNameToIdMap.value(productName, "");

                if (productId.isEmpty()) {
                    qWarning() << "Product not found:" << productName;
                    continue;
                }

                QString itemKey = QString("item%1").arg(baseIndex + i + 1);
                QJsonObject itemData;
                itemData["id_product"] = productId;
                itemData["product_count"] = item["quantity"].toInt();

                QUrl itemUrl(QString("https://qtrestaraunt-default-rtdb.firebaseio.com/order-items/%1/%2.json")
                                 .arg(orderId, itemKey));

                QNetworkRequest itemRequest(itemUrl);
                itemRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

                QNetworkReply *itemReply = m_networkManager->put(
                    itemRequest,
                    QJsonDocument(itemData).toJson()
                    );

                connect(itemReply, &QNetworkReply::finished, this, [=]() mutable {
                    itemReply->deleteLater();
                    addedCount++;

                    if (itemReply->error() != QNetworkReply::NoError) {
                        qWarning() << "Failed to add item:" << itemReply->errorString();
                    }

                    if (addedCount == items.count()) {
                        qDebug() << "Successfully added all items for order:" << orderId;
                        emit orderItemsAdded();
                    }
                });
            }
        });
    });
}

void databaseHandler::addOrderItemsForQML(const QString &table_num, const QVariantList &items) {
    if (!m_networkManager) {
        //emit orderItemsAdded();
        return;
    }

    // 1. Сначала получаем все продукты
    QNetworkRequest productsRequest(QUrl("https://qtrestaraunt-default-rtdb.firebaseio.com/products.json"));
    QNetworkReply *productsReply = m_networkManager->get(productsRequest);

    connect(productsReply, &QNetworkReply::finished, this, [=]() {
        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> productsGuard(productsReply);

        if (productsReply->error() != QNetworkReply::NoError) {
            emit errorOccurred("Ошибка загрузки продуктов: " + productsReply->errorString());
            //emit orderItemsAdded();
            return;
        }

        QJsonParseError parseError;
        QJsonDocument productsDoc = QJsonDocument::fromJson(productsReply->readAll(), &parseError);
        if (parseError.error != QJsonParseError::NoError) {
            emit errorOccurred("Ошибка парсинга продуктов: " + parseError.errorString());
            //emit orderItemsAdded();
            return;
        }

        QJsonObject products = productsDoc.object();
        QMap<QString, QString> productNameToIdMap;

        // Создаем карту соответствия имени продукта к его ID
        for (const QString &productId : products.keys()) {
            QJsonObject product = products[productId].toObject();
            QString productName = product["product_name"].toString();
            productNameToIdMap[productName] = productId;
        }

        // 2. Теперь находим заказ для стола
        QNetworkRequest tablesRequest(QUrl("https://qtrestaraunt-default-rtdb.firebaseio.com/tables.json"));
        QNetworkReply *tablesReply = m_networkManager->get(tablesRequest);

        connect(tablesReply, &QNetworkReply::finished, this, [=]() mutable {
            QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> tablesGuard(tablesReply);

            if (tablesReply->error() != QNetworkReply::NoError) {
                emit errorOccurred("Ошибка загрузки столов: " + tablesReply->errorString());
                //emit orderItemsAdded();
                return;
            }

            QJsonDocument tablesDoc = QJsonDocument::fromJson(tablesReply->readAll());
            QString orderNum;

            // Ищем заказ для указанного стола
            for (const QString &tableKey : tablesDoc.object().keys()) {
                QJsonObject table = tablesDoc.object()[tableKey].toObject();
                if (table["table_num"] == table_num) {
                    orderNum = table["order_num"].toString();
                    break;
                }
            }

            if (orderNum.isEmpty() || orderNum == "NULL") {
                //emit tableOrderNotFound();
                //emit orderItemsAdded();
                return;
            }

            // 3. Добавляем items с правильными product_id
            QNetworkRequest orderItemsRequest(QUrl(
                QString("https://qtrestaraunt-default-rtdb.firebaseio.com/order-items/%1.json").arg(orderNum)));
            QNetworkReply *orderItemsReply = m_networkManager->get(orderItemsRequest);

            connect(orderItemsReply, &QNetworkReply::finished, this, [=]() mutable {
                QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> itemsGuard(orderItemsReply);

                int baseIndex = 0;
                if (orderItemsReply->error() == QNetworkReply::NoError) {
                    QJsonDocument doc = QJsonDocument::fromJson(orderItemsReply->readAll());
                    if (doc.isObject()) {
                        baseIndex = doc.object().count();
                    }
                }

                // Добавляем каждый item
                int addedCount = 0;
                for (int i = 0; i < items.count(); ++i) {
                    QVariantMap item = items[i].toMap();
                    QString productName = item["name"].toString();
                    QString productId = productNameToIdMap.value(productName, "");

                    if (productId.isEmpty()) {
                        //emit productNotFound(productName);
                        continue;
                    }

                    QString itemKey = QString("item%1").arg(baseIndex + i + 1);
                    QJsonObject itemData;
                    itemData["id_product"] = productId;
                    itemData["product_count"] = item["quantity"].toInt();

                    QUrl itemUrl(QString("https://qtrestaraunt-default-rtdb.firebaseio.com/order-items/%1/%2.json")
                                     .arg(orderNum, itemKey));

                    QNetworkRequest itemRequest(itemUrl);
                    itemRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

                    QNetworkReply *itemReply = m_networkManager->put(
                        itemRequest,
                        QJsonDocument(itemData).toJson()
                        );

                    connect(itemReply, &QNetworkReply::finished, this, [=]() mutable {
                        itemReply->deleteLater();
                        addedCount++;

                        if (itemReply->error() != QNetworkReply::NoError) {
                            qWarning() << "Ошибка добавления item:" << itemReply->errorString();
                        }

                        if (addedCount == items.count()) {
                            emit orderItemsAdded();
                        }
                    });
                }
            });
        });
    });
}

