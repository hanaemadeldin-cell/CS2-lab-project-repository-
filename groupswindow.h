#ifndef GROUPSWINDOW_H
#define GROUPSWINDOW_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QTime>
#include <QSplitter>
#include "database.h"

class GroupsWindow : public QWidget
{
    Q_OBJECT

public:
    explicit GroupsWindow(const QString& myUsername, Database* db, QWidget* parent = nullptr);

    void appendGroupMessage(const QString& groupName,
                            const QString& sender,
                            const QString& text);

signals:
    void groupMessageRequested(const QString& groupName, const QString& text);
    void joinGroupRequested(const QString& groupName);

private slots:
    void on_createButton_clicked();
    void on_joinButton_clicked();
    void on_sendButton_clicked();
    void on_groupSelected(QListWidgetItem* item);
    void on_backButton_clicked();

private:
    QString      m_myUsername;
    Database*    m_db;
    QString      m_currentGroup;

    QListWidget* m_groupList;
    QListWidget* m_memberList;
    QTextEdit*   m_chatArea;
    QLineEdit*   m_groupNameEdit;
    QLineEdit*   m_messageEdit;
    QPushButton* m_createButton;
    QPushButton* m_joinButton;
    QPushButton* m_sendButton;
    QPushButton* m_backButton;
    QLabel*      m_currentGroupLabel;

    void loadGroups();
    void loadGroupHistory(const QString& groupName);
    void loadGroupMembers(const QString& groupName);
};

#endif // GROUPSWINDOW_H
