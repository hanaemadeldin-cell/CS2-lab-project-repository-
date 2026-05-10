#ifndef NETWORKCLIENT_H
#define NETWORKCLIENT_H

#include <QObject>
#include <QString>
#include <QTcpSocket>
#include <QAbstractSocket>

class INetworkClient : public QObject {
    Q_OBJECT
public:
    explicit INetworkClient(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~INetworkClient() {}
    virtual void connectToServer(const QString& ip) = 0;
    virtual void sendMessage(const QString& msg) = 0;
    virtual void setUsername(const QString& user) = 0;

signals:
    void messageReceived(QString user, QString text);
    void statusUpdated(QString status);
};

class MockNetworkClient : public INetworkClient {
    Q_OBJECT
public:
    void connectToServer(const QString& ip) override;
    void sendMessage(const QString& msg) override;
    void setUsername(const QString& user) override;
private:
    QString username;
};

class RealNetworkClient : public INetworkClient {
    Q_OBJECT
public:
    explicit RealNetworkClient(QObject* parent = nullptr);
    void connectToServer(const QString& ip) override;
    void sendMessage(const QString& msg) override;
    void setUsername(const QString& user) override;

private slots:
    void onConnected();
    void onReadyRead();
    void onError(QAbstractSocket::SocketError socketError);

private:
    QString username;
    QTcpSocket* socket;
};

#endif
