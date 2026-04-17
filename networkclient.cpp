#include "networkclient.h"
#include <QTimer>
#include "networkclient.h"
#include <QJsonObject>
#include <QJsonDocument>


void MockNetworkClient::connectToServer(const QString& ip) {
    QTimer::singleShot(1000, this, [this, ip]() {
        emit statusUpdated("Connected to Mock Server at " + ip);
    });
}

void MockNetworkClient::sendMessage(const QString& msg) {
    emit messageReceived("Me", msg);

    QTimer::singleShot(1500, this, [this]() {
        emit messageReceived("Server", "Mock response: Received your message!");
    });
}

RealNetworkClient::RealNetworkClient(QObject* parent)
    : INetworkClient(parent)
{
    socket = new QTcpSocket(this);

    connect(socket, &QTcpSocket::connected, this, &RealNetworkClient::onConnected);
    connect(socket, &QTcpSocket::readyRead, this, &RealNetworkClient::onReadyRead);
    connect(socket, &QTcpSocket::errorOccurred, this, &RealNetworkClient::onError);
}

void RealNetworkClient::connectToServer(const QString& ip) {
    socket->connectToHost(ip, 54321);
}

void RealNetworkClient::sendMessage(const QString& msg) {
    QJsonObject json;
    json["type"] = "message";
    json["sender"] = "Me";
    json["payload"] = msg;

    QJsonDocument doc(json);
    QByteArray data = doc.toJson(QJsonDocument::Compact);

    socket->write(data);
}

void RealNetworkClient::onConnected() {
    emit statusUpdated("Connected to REAL server");
}

void RealNetworkClient::onReadyRead() {
    QByteArray data = socket->readAll();
    emit messageReceived("Server", QString::fromUtf8(data));
}

void RealNetworkClient::onError(QAbstractSocket::SocketError) {
    emit statusUpdated("Connection error: " + socket->errorString());
}
