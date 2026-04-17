#ifndef NETWORKCLIENT_H
#define NETWORKCLIENT_H

#include <QObject>
#include <QString>

// Abstract Class (Requirement)
class INetworkClient : public QObject {
    Q_OBJECT
public:
    virtual ~INetworkClient() {}
    virtual void connectToServer(const QString& ip) = 0;
    virtual void sendMessage(const QString& msg) = 0;
};

// Mocked Client (Requirement)
class MockNetworkClient : public INetworkClient {
    Q_OBJECT

public:
    void connectToServer(const QString& ip) override;
    void sendMessage(const QString& msg) override;

signals:
    void statusUpdated(QString status);
    void messageReceived(QString user, QString text);
};

#endif
