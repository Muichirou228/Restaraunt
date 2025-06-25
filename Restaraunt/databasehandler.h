#ifndef DATABASEHANDLER_H
#define DATABASEHANDLER_H

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QList>
#include <QVariantList>
#include <QVariantMap>

class databaseHandler : public QObject
{
    Q_OBJECT

    Q_PROPERTY (QList<int> bookedTables READ bookedTables NOTIFY bookingsReady)
    //Q_PROPERTY(QStringList productList READ productList NOTIFY productsLoaded)

public:
    explicit databaseHandler(QObject *parent = nullptr);

    Q_INVOKABLE void getBookingsOnDate(const QString &date);
    Q_INVOKABLE void clearBookings();
    QList<int> bookedTables() const;
    Q_INVOKABLE void checkIfUserCodeExists(QString code);
    Q_INVOKABLE void addNewBooking(const QString date, QList<int> tables, QString id_client);
    Q_INVOKABLE void checkIfClientExists(QString firstName, QString secondName, QString thirdName, int age);
    Q_INVOKABLE void addNewClient (QString firstName, QString secondName, QString thirdName, int age);
    Q_INVOKABLE QList<int> convertToListFromString(QString text);
    Q_INVOKABLE QString checkTableStatus(QString table_num);

    Q_INVOKABLE void getTableOrders(int table_num);

    Q_INVOKABLE void loadProducts();
    Q_INVOKABLE QStringList productList() const { return m_productList; }

signals:
    void bookingsReady(const QList<int> &bookedTables);
    void clientChecked(const QString &id_client);
    void clientAdded(const QString &id_client);
    void bookingAdded(bool result, QString message);
    void codeChecked(QString code);
    void tableStatusChecked(QString table_num, QString status);
    void productsLoaded();
    void tableOrdersReady(const QVariantList &orders);
    void errorOccurred(const QString& error);
private slots:
    void handleNetworkReply(QNetworkReply *reply);
private:
    QNetworkAccessManager *m_networkManager;
    QList<int> m_bookedTables;
    int m_currentTableNumber;

    QVariantList m_currentOrders;
    QVariantMap m_currentProducts;

    void fetchOrderItems(const QString &orderId);
    void fetchProductsInfo(const QVariantList &productRequests);

    QStringList m_productList;
};

#endif // DATABASEHANDLER_H
