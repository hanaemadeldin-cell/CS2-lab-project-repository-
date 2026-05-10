#include "networkclient.h"
#include <QTimer>
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

void MockNetworkClient::setUsername(const QString& user) {
    username = user;
}

RealNetworkClient::RealNetworkClient(QObject* parent)
    : INetworkClient(parent)
{
    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::connected,      this, &RealNetworkClient::onConnected);
    connect(socket, &QTcpSocket::readyRead,      this, &RealNetworkClient::onReadyRead);
    connect(socket, &QTcpSocket::errorOccurred,  this, &RealNetworkClient::onError);
}

void RealNetworkClient::connectToServer(const QString& ip) {
    socket->connectToHost(ip, 54321);
}

void RealNetworkClient::setUsername(const QString& user) {
    username = user;
}

void RealNetworkClient::sendMessage(const QString& msg) {
    if (socket->state() != QAbstractSocket::ConnectedState) return;

    // If msg is already valid JSON, send it directly (private/group messages)
    QJsonDocument check = QJsonDocument::fromJson(msg.toUtf8());
    if (!check.isNull() && check.isObject()) {
        socket->write(msg.toUtf8() + "\n");
        socket->flush();
        return;
    }

    // Otherwise wrap as a chat message
    QJsonObject obj;
    obj["type"]    = "chat";
    obj["sender"]  = username;
    obj["payload"] = msg;
    socket->write(QJsonDocument(obj).toJson(QJsonDocument::Compact) + "\n");
    socket->flush();
}

void RealNetworkClient::onConnected() {
    // Send login message so server registers this user
    QJsonObject loginMsg;
    loginMsg["type"]    = "login";
    loginMsg["sender"]  = username;
    loginMsg["payload"] = "";
    socket->write(QJsonDocument(loginMsg).toJson(QJsonDocument::Compact) + "\n");
    socket->flush();

    emit statusUpdated("Connected to server");
}

void RealNetworkClient::onReadyRead() {
    while (socket->canReadLine()) {
        QByteArray line = socket->readLine().trimmed();
        if (line.isEmpty()) continue;

        QJsonDocument doc = QJsonDocument::fromJson(line);
        if (!doc.isObject()) {
            emit messageReceived("Server", QString::fromUtf8(line));
            continue;
        }

        QJsonObject obj = doc.object();
        QString type    = obj.value("type").toString();
        QString sender  = obj.value("sender").toString("Server");
        QString payload = obj.value("payload").toString();

        if (type == "chat") {
            emit messageReceived(sender, payload);
        } else if (type == "status") {
            emit statusUpdated(obj.value("message").toString());
        } else if (type == "error") {
            emit statusUpdated("Error: " + obj.value("message").toString());
        } else {
            // Pass all other types (private, group, user_list) as raw JSON
            emit messageReceived(sender, payload);
        }
    }
}

void RealNetworkClient::onError(QAbstractSocket::SocketError) {
    if (socket->error() != QAbstractSocket::RemoteHostClosedError)
        emit statusUpdated("Connection error: " + socket->errorString());
}
