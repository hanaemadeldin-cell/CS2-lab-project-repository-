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
    , m_usersWindow(nullptr)
    , m_groupsWindow(nullptr)
{
    ui->setupUi(this);
    setWindowTitle("Chat - " + username);
    resize(750, 520);

    m_usersWindow  = new UsersWindow(username);
    m_groupsWindow = new GroupsWindow(username, m_db);

    if (m_db->open()) loadGlobalHistory();

    connect(m_usersWindow, &UsersWindow::privateChatRequested,
            this, &Chat::onPrivateChatRequested);
    connect(m_groupsWindow, &GroupsWindow::joinGroupRequested,
            this, &Chat::onJoinGroupRequested);
    connect(m_groupsWindow, &GroupsWindow::groupMessageRequested,
            this, &Chat::onGroupMessageRequested);

    // Connect buttons manually to be safe
    connect(ui->userspushButton,   &QPushButton::clicked, this, &Chat::on_userspushButton_clicked);
    connect(ui->groupspushButton,  &QPushButton::clicked, this, &Chat::on_groupspushButton_clicked);
    connect(ui->settingspushButton,&QPushButton::clicked, this, &Chat::on_settingspushButton_clicked);
    connect(ui->SendpushButton,    &QPushButton::clicked, this, &Chat::on_SendpushButton_clicked);
    connect(ui->ClearpushButton_2, &QPushButton::clicked, this, &Chat::on_ClearpushButton_2_clicked);
    connect(ui->messagelineEdit,   &QLineEdit::returnPressed, this, &Chat::on_SendpushButton_clicked);

    client = new RealNetworkClient(this);
    client->setUsername(username);
    client->connectToServer("127.0.0.1");

    connect(client, &INetworkClient::messageReceived, this, &Chat::onIncomingMessage);
    connect(client, &INetworkClient::statusUpdated,   this, &Chat::onStatusUpdated);
}

Chat::~Chat()
{
    delete m_usersWindow;
    delete m_groupsWindow;
    delete m_db;
    delete ui;
}

void Chat::onIncomingMessage(QString user, QString text)
{
    QString time = QTime::currentTime().toString("hh:mm");
    ui->chattextEdit->append("<span style='color:#888;'>" + time +
                             "</span> <b>" + user + "</b>: " + text);
    if (m_db->isOpen()) m_db->saveMessage(user, "global", text, "global");
}

void Chat::onStatusUpdated(QString status)
{
    ui->statusLabel->setText(status);
}

void Chat::on_SendpushButton_clicked()
{
    ChatLogic logic;
    QString message = ui->messagelineEdit->text();
    if (!logic.validateMessage(message)) {
        QMessageBox::warning(this, "Error", "Message cannot be empty");
        return;
    }
    QString time = QTime::currentTime().toString("hh:mm");
    ui->chattextEdit->append("<span style='color:#888;'>" + time +
                             "</span> <b>" + username + "</b>: " + message);
    client->sendMessage(message);
    if (m_db->isOpen()) m_db->saveMessage(username, "global", message, "global");
    ui->messagelineEdit->clear();
}

// Clears the chat display area
void Chat::on_ClearpushButton_2_clicked()
{
    ui->chattextEdit->setPlainText("");
    ui->messagelineEdit->clear();
}

void Chat::on_settingspushButton_clicked()
{
    Settings *s = new Settings(this);
    hide();
    s->show();
}

// Opens the Users window (separate screen showing online users)
void Chat::on_userspushButton_clicked()
{
    m_usersWindow->show();
    m_usersWindow->raise();
    m_usersWindow->activateWindow();
}

// Opens the Groups window (separate screen for group chat)
void Chat::on_groupspushButton_clicked()
{
    m_groupsWindow->show();
    m_groupsWindow->raise();
    m_groupsWindow->activateWindow();
}

void Chat::onPrivateChatRequested(const QString& targetUser)
{
    if (!m_privateChats.contains(targetUser)) {
        auto* win = new PrivateChatWindow(username, targetUser, m_db);
        connect(win, &PrivateChatWindow::privateMessageRequested,
                this, &Chat::onPrivateMessageRequested);
        m_privateChats[targetUser] = win;
    }
    m_privateChats[targetUser]->show();
    m_privateChats[targetUser]->raise();
    m_privateChats[targetUser]->activateWindow();
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

void Chat::onJoinGroupRequested(const QString& groupName)
{
    QJsonObject obj;
    obj["type"]   = "join_group";
    obj["sender"] = username;
    obj["group"]  = groupName;
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
    if (m_db->isOpen()) m_db->addUserToGroup(groupName, username);
    m_groupsWindow->show();
    m_groupsWindow->raise();
    onJoinGroupRequested(groupName);
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
                       ? obj.value("recipient").toString() : sender;
        if (!m_privateChats.contains(peer)) {
            auto* win = new PrivateChatWindow(username, peer, m_db);
            connect(win, &PrivateChatWindow::privateMessageRequested,
                    this, &Chat::onPrivateMessageRequested);
            m_privateChats[peer] = win;
        }
        m_privateChats[peer]->appendIncoming(sender, payload);
        m_privateChats[peer]->show();
        m_privateChats[peer]->raise();
    }
    else if (type == "group_message") {
        QString group = obj.value("group").toString();
        if (m_db->isOpen()) m_db->saveMessage(sender, group, payload, "group");
        if (m_groupsWindow) m_groupsWindow->appendGroupMessage(group, sender, payload);
    }
    else if (type == "user_list") {
        QJsonArray arr = obj.value("users").toArray();
        QStringList users;
        for (const QJsonValue& v : arr) users << v.toString();
        if (m_usersWindow) m_usersWindow->updateUserList(users);
    }
}

void Chat::appendMessage(const QString& line)
{
    ui->chattextEdit->append(line);
}

void Chat::loadGlobalHistory()
{
    auto history = m_db->getGlobalHistory(50);
    if (history.isEmpty()) return;
    ui->chattextEdit->append("<span style='color:#aaa;'>── Previous Messages ──</span>");
    for (const ChatMessage& m : history)
        ui->chattextEdit->append("<span style='color:#888;'>" +
            m.timestamp.toString("hh:mm") + "</span> <b>" +
            m.sender + "</b>: " + m.text);
    ui->chattextEdit->append("<span style='color:#aaa;'>──────────────────────</span>");
}
