#ifndef DATABASE_H
#define DATABASE_H

#include <QString>
#include <QList>
#include <QDateTime>
#include <QtSql/QSqlDatabase>

struct ChatMessage {
    QString sender;
    QString recipient;   // "global", a username, or a group name
    QString text;
    QString chatType;    // "global" | "private" | "group"
    QDateTime timestamp;
};

// Pure data layer — no GUI, no networking code inside
class Database
{
public:
    explicit Database(const QString& dbPath = "chat_history.db");
    ~Database();

    bool open();
    void close();
    bool isOpen() const;

    // Messages
    bool saveMessage(const QString& sender, const QString& recipient,
                     const QString& text, const QString& chatType);
    QList<ChatMessage> getGlobalHistory(int limit = 100) const;
    QList<ChatMessage> getPrivateHistory(const QString& user1,
                                          const QString& user2,
                                          int limit = 100) const;
    QList<ChatMessage> getGroupHistory(const QString& groupName,
                                        int limit = 100) const;

    // Groups
    bool createGroup(const QString& groupName, const QString& creator);
    bool addUserToGroup(const QString& groupName, const QString& username);
    bool removeUserFromGroup(const QString& groupName, const QString& username);
    QStringList getGroupMembers(const QString& groupName) const;
    QStringList getAllGroups() const;

private:
    bool createTables();
    QSqlDatabase m_db;
};

#endif // DATABASE_H
