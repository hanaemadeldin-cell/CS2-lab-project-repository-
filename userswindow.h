#ifndef USERSWINDOW_H
#define USERSWINDOW_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

// Separate screen that shows all online users.
// User can select one and start a private chat.
// Spec: "users' names should be displayed on a separate screen"
class UsersWindow : public QWidget
{
    Q_OBJECT

public:
    explicit UsersWindow(const QString& myUsername, QWidget* parent = nullptr);

    // Called whenever the server sends an updated user list
    void updateUserList(const QStringList& users);

signals:
    // Emitted when user clicks "Private Chat" with a selected user
    void privateChatRequested(const QString& targetUser);

private slots:
    void on_privateChatButton_clicked();
    void on_backButton_clicked();

private:
    QString        m_myUsername;
    QListWidget*   m_userList;
    QPushButton*   m_privateChatButton;
    QPushButton*   m_backButton;
    QLabel*        m_titleLabel;
    QLabel*        m_hintLabel;
};

#endif // USERSWINDOW_H
