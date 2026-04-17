#include "chatlogic.h"

bool ChatLogic::validateUsername(const QString& username) {
    return !username.trimmed().isEmpty();
}

bool ChatLogic::validateMessage(const QString& message) {
   return !message.trimmed().isEmpty();
}

QString ChatLogic::formatMessage(const QString& user, const QString& message) {
    return user + ": " + message;
}
