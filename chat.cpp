#include "chat.h"
#include "ui_chat.h"
#include "loginwindow.h"
#include "chatlogic.h"
#include "settings.h"
#include <QTime>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

Chat::Chat(QString username, QWidget *parent)
    : QWidget(parent)
    , username(username)
    , ui(new Ui::Chat)
    , m_db(new Database("chat_history.db"))
    , m_usersWindow(new UsersWindow(username, this))
    , m_groupsWindow(new GroupsWindow(username, m_db, this))
{
    ui->setupUi(this);
    setWindowTitle("Chat - " + username);

    if (m_db->open())
        loadGlobalHistory();

    connect(m_usersWindow, &UsersWindow::privateChatRequested,
            this, &Chat::onPrivateChatRequested);
    connect(m_groupsWindow, &GroupsWindow::joinGroupRequested,
            this, &Chat::onJoinGroupRequested);


    connect(m_groupsWindow, &GroupsWindow::groupMessageRequested,
            this, &Chat::onGroupMessageRequested);

    client = new RealNetworkClient(this);
    client->setUsername(username);
    client->connectToServer("127.0.0.1");

    connect(client, &INetworkClient::messageReceived,
            this, &Chat::onIncomingMessage);
    connect(client, &INetworkClient::statusUpdated,
            this, &Chat::onStatusUpdated);
}

Chat::~Chat()
{
    delete m_db;
    delete ui;
}

void Chat::onIncomingMessage(QString user, QString text)
{
    QString time = QTime::currentTime().toString("hh:mm");
    appendMessage(time + " - " + user + ": " + text);
    if (m_db->isOpen())
        m_db->saveMessage(user, "global", text, "global");
}

void Chat::onStatusUpdated(QString status)
{
    appendMessage("[STATUS] " + status);
}

void Chat::on_SendpushButton_clicked()
{
    ChatLogic logic;
    QString message = ui->messagelineEdit->text();
    if (!logic.validateMessage(message)) {
        QMessageBox::warning(this, "Error", "Message cannot be empty");
        return;
    }
    QString time      = QTime::currentTime().toString("hh:mm");
    QString formatted = logic.formatMessage(username, message);
    appendMessage(time + " - " + formatted);
    client->sendMessage(message);
    if (m_db->isOpen())
        m_db->saveMessage(username, "global", message, "global");
    ui->messagelineEdit->clear();
}

void Chat::on_ClearpushButton_2_clicked()
{
    ui->messagelineEdit->clear();
}

void Chat::on_settingspushButton_clicked()
{
    Settings *s = new Settings(this);
    hide();
    s->show();
}

void Chat::on_userspushButton_clicked()
{
    m_usersWindow->show();
}

void Chat::on_groupspushButton_clicked()
{
    m_groupsWindow->show();
}

void Chat::onPrivateChatRequested(const QString& targetUser)
{
    if (!m_privateChats.contains(targetUser)) {
        auto* win = new PrivateChatWindow(username, targetUser, m_db, this);
        connect(win, &PrivateChatWindow::privateMessageRequested,
                this, &Chat::onPrivateMessageRequested);
        m_privateChats[targetUser] = win;
    }
    m_privateChats[targetUser]->show();
}

void Chat::onPrivateMessageRequested(const QString& recipient, const QString& text)
{
    QJsonObject obj;
    obj["type"]      = "private_message";
    obj["sender"]    = username;
    obj["recipient"] = recipient;
    obj["payload"]   = text;
    client->sendMessage(QJsonDocument(obj).toJson(QJsonDocument::Compact));
}

void Chat::onGroupMessageRequested(const QString& groupName, const QString& text)
{
    QJsonObject obj;
    obj["type"]    = "group_message";
    obj["sender"]  = username;
    obj["group"]   = groupName;
    obj["payload"] = text;
    client->sendMessage(QJsonDocument(obj).toJson(QJsonDocument::Compact));
}

void Chat::joinGroup(const QString& groupName)
{
    if (m_db->isOpen())
        m_db->addUserToGroup(groupName, username);
    m_groupsWindow->show();
    QJsonObject obj;
    obj["type"]   = "join_group";
    obj["sender"] = username;
    obj["group"]  = groupName;
    client->sendMessage(QJsonDocument(obj).toJson(QJsonDocument::Compact));
}

void Chat::handleIncomingJson(const QString& raw)
{
    QJsonDocument doc = QJsonDocument::fromJson(raw.toUtf8());
    if (!doc.isObject()) return;
    QJsonObject obj = doc.object();
    QString type    = obj.value("type").toString();
    QString sender  = obj.value("sender").toString();
    QString payload = obj.value("payload").toString();

    if (type == "private_message") {
        QString peer = (sender == username)
                       ? obj.value("recipient").toString()
                       : sender;
        if (!m_privateChats.contains(peer)) {
            auto* win = new PrivateChatWindow(username, peer, m_db, this);
            connect(win, &PrivateChatWindow::privateMessageRequested,
                    this, &Chat::onPrivateMessageRequested);
            m_privateChats[peer] = win;
        }
        m_privateChats[peer]->appendIncoming(sender, payload);
        if (!m_privateChats[peer]->isVisible())
            m_privateChats[peer]->show();
    }
    else if (type == "group_message") {
        QString group = obj.value("group").toString();
        if (m_db->isOpen())
            m_db->saveMessage(sender, group, payload, "group");
        m_groupsWindow->appendGroupMessage(group, sender, payload);
    }
    else if (type == "user_list") {
        QJsonArray arr = obj.value("users").toArray();
        QStringList users;
        for (const QJsonValue& v : arr) users << v.toString();
        m_usersWindow->updateUserList(users);
    }
}

void Chat::appendMessage(const QString& line)
{
    ui->chattextEdit->insertPlainText(line + "\n");
}

void Chat::loadGlobalHistory()
{
    auto history = m_db->getGlobalHistory(50);
    if (history.isEmpty()) return;
    appendMessage("--- Previous Messages ---");
    for (const ChatMessage& m : history)
        appendMessage(m.timestamp.toString("hh:mm") + " - " + m.sender + ": " + m.text);
    appendMessage("--- End of History ---");
}

void Chat::onJoinGroupRequested(const QString& groupName)
{
    QJsonObject obj;
    obj["type"]   = "join_group";
    obj["sender"] = username;
    obj["group"]  = groupName;
    client->sendMessage(QJsonDocument(obj).toJson(QJsonDocument::Compact));
}
