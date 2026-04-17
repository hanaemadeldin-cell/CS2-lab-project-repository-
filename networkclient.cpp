#include "networkclient.h"
#include <QTimer>

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
