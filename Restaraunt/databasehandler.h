#ifndef DATABASEHANDLER_H
#define DATABASEHANDLER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QList>

class databaseHandler : public QObject
{
    Q_OBJECT

    Q_PROPERTY (QList<int> bookedTables READ bookedTables NOTIFY bookingsReady)

public:
    explicit databaseHandler(QObject *parent = nullptr);

    Q_INVOKABLE void getBookingsOnDate(const QString &date);
    QList<int> bookedTables() const;

signals:
    void bookingsReady(const QList<int> &bookedTables);
private slots:
    void handleNetworkReply(QNetworkReply *reply);
private:
    QNetworkAccessManager *m_networkManager;
    QList<int> m_bookedTables;
};

#endif // DATABASEHANDLER_H
