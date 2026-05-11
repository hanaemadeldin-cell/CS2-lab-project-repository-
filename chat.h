#ifndef CHAT_H
#define CHAT_H

#include <QWidget>
#include <QMap>
#include "networkclient.h"
#include "database.h"
#include "userswindow.h"
#include "groupswindow.h"
#include "privatechatwindow.h"

namespace Ui { class Chat; }

class Chat : public QWidget
{
    Q_OBJECT

public:
    Chat(QString username, QWidget *parent = nullptr);
    ~Chat();

    QString username;
    Ui::Chat *ui;

    void joinGroup(const QString& groupName);
    void on_userspushButton_clicked();
    void on_groupspushButton_clicked();

private slots:
    void on_SendpushButton_clicked();
    void on_ClearpushButton_2_clicked();
    void on_settingspushButton_clicked();

    void onIncomingMessage(QString user, QString text);
    void onStatusUpdated(QString status);

    void onPrivateChatRequested(const QString& targetUser);
    void onJoinGroupRequested(const QString& groupName);
    void onGroupMessageRequested(const QString& groupName, const QString& text);
    void onPrivateMessageRequested(const QString& recipient, const QString& text);

private:
    INetworkClient*   client;
    Database*         m_db;
    UsersWindow*      m_usersWindow;
    GroupsWindow*     m_groupsWindow;

    QMap<QString, PrivateChatWindow*> m_privateChats;

    void appendMessage(const QString& line);
    void loadGlobalHistory();
    void handleIncomingJson(const QString& raw);
};

#endif // CHAT_H
