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
    QJsonObject obj;
    obj["type"] = "message";
    obj["sender"] = username;
    obj["payload"] = msg;

    QJsonDocument doc(obj);
    socket->write(doc.toJson());
}

void RealNetworkClient::setUsername(const QString& user) {
    username = user;
}
void RealNetworkClient::onConnected() {
    emit statusUpdated("Connected to REAL server");
}

void RealNetworkClient::onReadyRead() {
    QByteArray data = socket->readAll();

    // remove "Echo: " prefix
    QString response = QString::fromUtf8(data);

    QJsonDocument doc = QJsonDocument::fromJson(response.toUtf8());

    if (doc.isObject()) {
        QJsonObject obj = doc.object();
        QString payload = obj["payload"].toString();

        emit messageReceived("Server", payload);
    } else {
        emit messageReceived("Server", response);
    }
}

void RealNetworkClient::onError(QAbstractSocket::SocketError) {
    if (socket->error() != QAbstractSocket::RemoteHostClosedError) {
        emit statusUpdated("Connection error: " + socket->errorString());
    }
}
