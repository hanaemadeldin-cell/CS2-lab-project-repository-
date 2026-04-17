#ifndef CHATLOGIC_H
#define CHATLOGIC_H

#include <QString>

class ChatLogic {
public:
    bool validateUsername(const QString& username);
    bool validateMessage(const QString& message);
    QString formatMessage(const QString& user, const QString& message);
};

#endif
