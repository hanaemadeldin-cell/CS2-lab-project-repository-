#ifndef NETWORKCLIENT_H
#define NETWORKCLIENT_H

#include <QTcpSocket>
#include <QString>

class NetworkClient {
public:
    NetworkClient();

    void connectToServer();
    void sendMessage(QString message);

private:
    QTcpSocket* socket;
};

#endif
