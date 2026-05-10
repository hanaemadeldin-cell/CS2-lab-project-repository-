#ifndef PRIVATECHATWINDOW_H
#define PRIVATECHATWINDOW_H

#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTime>
#include "database.h"

// Window for a private one-to-one conversation
class PrivateChatWindow : public QWidget
{
    Q_OBJECT

public:
    explicit PrivateChatWindow(const QString& myUsername,
                                const QString& peerUsername,
                                Database* db,
                                QWidget* parent = nullptr);

    // Called by chat.cpp when an incoming private message arrives for this peer
    void appendIncoming(const QString& sender, const QString& text);

signals:
    void privateMessageRequested(const QString& recipient, const QString& text);

private slots:
    void on_sendButton_clicked();
    void on_backButton_clicked();

private:
    QString    m_myUsername;
    QString    m_peerUsername;
    Database*  m_db;

    QTextEdit*   m_chatArea;
    QLineEdit*   m_messageEdit;
    QPushButton* m_sendButton;
    QPushButton* m_backButton;

    void loadHistory();
};

#endif // PRIVATECHATWINDOW_H
