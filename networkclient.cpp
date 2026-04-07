#include "NetworkClient.h"
#include <QDebug>

NetworkClient::NetworkClient() {
    socket = new QTcpSocket();
}

void NetworkClient::connectToServer() {
    socket->connectToHost("127.0.0.1", 12345);

    if (socket->waitForConnected()) {
        qDebug() << "Connected to server!";
    } else {
        qDebug() << "Connection failed!";
    }
}

void NetworkClient::sendMessage(QString message) {
    socket->write(message.toUtf8());
    socket->flush();

    qDebug() << "Sent:" << message;
}
