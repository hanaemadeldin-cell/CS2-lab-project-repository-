#include "database.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>

Database::Database(const QString& dbPath)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(dbPath);
}

Database::~Database() { close(); }

bool Database::open()
{
    if (!m_db.open()) {
        qWarning() << "DB open failed:" << m_db.lastError().text();
        return false;
    }
    return createTables();
}

void Database::close() { if (m_db.isOpen()) m_db.close(); }
bool Database::isOpen() const { return m_db.isOpen(); }

bool Database::createTables()
{
    QSqlQuery q;
    bool ok = q.exec(R"(
        CREATE TABLE IF NOT EXISTS messages (
            id        INTEGER PRIMARY KEY AUTOINCREMENT,
            sender    TEXT NOT NULL,
            recipient TEXT NOT NULL,
            text      TEXT NOT NULL,
            chat_type TEXT NOT NULL DEFAULT 'global',
            timestamp DATETIME DEFAULT CURRENT_TIMESTAMP
        )
    )");
    if (!ok) { qWarning() << "createTables messages:" << q.lastError().text(); return false; }

    ok = q.exec(R"(
        CREATE TABLE IF NOT EXISTS groups (
            group_name TEXT PRIMARY KEY,
            creator    TEXT NOT NULL,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP
        )
    )");
    if (!ok) { qWarning() << "createTables groups:" << q.lastError().text(); return false; }

    ok = q.exec(R"(
        CREATE TABLE IF NOT EXISTS group_members (
            group_name TEXT NOT NULL,
            username   TEXT NOT NULL,
            PRIMARY KEY (group_name, username)
        )
    )");
    if (!ok) { qWarning() << "createTables group_members:" << q.lastError().text(); return false; }

    return true;
}

bool Database::saveMessage(const QString& sender, const QString& recipient,
                            const QString& text, const QString& chatType)
{
    QSqlQuery q;
    q.prepare("INSERT INTO messages (sender,recipient,text,chat_type) VALUES(?,?,?,?)");
    q.addBindValue(sender); q.addBindValue(recipient);
    q.addBindValue(text);   q.addBindValue(chatType);
    if (!q.exec()) { qWarning() << "saveMessage:" << q.lastError().text(); return false; }
    return true;
}

static ChatMessage rowToMsg(QSqlQuery& q) {
    ChatMessage m;
    m.sender    = q.value(0).toString();
    m.recipient = q.value(1).toString();
    m.text      = q.value(2).toString();
    m.chatType  = q.value(3).toString();
    m.timestamp = q.value(4).toDateTime();
    return m;
}

QList<ChatMessage> Database::getGlobalHistory(int limit) const
{
    QList<ChatMessage> result;
    QSqlQuery q;
    q.prepare("SELECT sender,recipient,text,chat_type,timestamp FROM messages "
              "WHERE chat_type='global' ORDER BY timestamp DESC LIMIT ?");
    q.addBindValue(limit);
    if (!q.exec()) return result;
    while (q.next()) result.prepend(rowToMsg(q));
    return result;
}

QList<ChatMessage> Database::getPrivateHistory(const QString& user1,
                                                const QString& user2, int limit) const
{
    QList<ChatMessage> result;
    QSqlQuery q;
    q.prepare("SELECT sender,recipient,text,chat_type,timestamp FROM messages "
              "WHERE chat_type='private' AND "
              "((sender=? AND recipient=?) OR (sender=? AND recipient=?)) "
              "ORDER BY timestamp DESC LIMIT ?");
    q.addBindValue(user1); q.addBindValue(user2);
    q.addBindValue(user2); q.addBindValue(user1);
    q.addBindValue(limit);
    if (!q.exec()) return result;
    while (q.next()) result.prepend(rowToMsg(q));
    return result;
}

QList<ChatMessage> Database::getGroupHistory(const QString& groupName, int limit) const
{
    QList<ChatMessage> result;
    QSqlQuery q;
    q.prepare("SELECT sender,recipient,text,chat_type,timestamp FROM messages "
              "WHERE chat_type='group' AND recipient=? ORDER BY timestamp DESC LIMIT ?");
    q.addBindValue(groupName); q.addBindValue(limit);
    if (!q.exec()) return result;
    while (q.next()) result.prepend(rowToMsg(q));
    return result;
}

bool Database::createGroup(const QString& groupName, const QString& creator)
{
    QSqlQuery q;
    q.prepare("INSERT OR IGNORE INTO groups(group_name,creator) VALUES(?,?)");
    q.addBindValue(groupName); q.addBindValue(creator);
    if (!q.exec()) return false;
    return addUserToGroup(groupName, creator);
}

bool Database::addUserToGroup(const QString& groupName, const QString& username)
{
    QSqlQuery q;
    q.prepare("INSERT OR IGNORE INTO group_members(group_name,username) VALUES(?,?)");
    q.addBindValue(groupName); q.addBindValue(username);
    return q.exec();
}

bool Database::removeUserFromGroup(const QString& groupName, const QString& username)
{
    QSqlQuery q;
    q.prepare("DELETE FROM group_members WHERE group_name=? AND username=?");
    q.addBindValue(groupName); q.addBindValue(username);
    return q.exec();
}

QStringList Database::getGroupMembers(const QString& groupName) const
{
    QStringList members;
    QSqlQuery q;
    q.prepare("SELECT username FROM group_members WHERE group_name=?");
    q.addBindValue(groupName);
    if (!q.exec()) return members;
    while (q.next()) members << q.value(0).toString();
    return members;
}

QStringList Database::getAllGroups() const
{
    QStringList groups;
    QSqlQuery q("SELECT group_name FROM groups ORDER BY group_name");
    while (q.next()) groups << q.value(0).toString();
    return groups;
}
