#include "userswindow.h"
#include <QMessageBox>

UsersWindow::UsersWindow(const QString& myUsername, QWidget* parent)
    : QWidget(parent, Qt::Window)
    , m_myUsername(myUsername)
{
    setWindowTitle("Online Users");
    setFixedSize(300, 400);
    setStyleSheet(
        "QWidget { background: #f5f5f5; font-family: -apple-system; font-size: 13px; }"
        "QListWidget { background: white; border: 1px solid #ddd; border-radius: 8px; padding: 4px; }"
        "QPushButton { background: #0084ff; color: white; border: none; border-radius: 8px; padding: 8px; font-weight: bold; }"
        "QPushButton#m_backButton { background: #e0e0e0; color: #333; }"
    );

    m_titleLabel = new QLabel("<b>Online Users</b>");
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setStyleSheet("font-size: 16px; margin-bottom: 4px;");

    m_hintLabel = new QLabel("Select a user to start a private chat");
    m_hintLabel->setAlignment(Qt::AlignCenter);
    m_hintLabel->setStyleSheet("color: gray; font-size: 11px;");

    m_userList = new QListWidget;
    m_userList->setSpacing(2);

    m_privateChatButton = new QPushButton("💬 Start Private Chat");
    m_backButton = new QPushButton("Close");
    m_backButton->setObjectName("m_backButton");

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(16, 16, 16, 16);
    mainLayout->setSpacing(10);
    mainLayout->addWidget(m_titleLabel);
    mainLayout->addWidget(m_hintLabel);
    mainLayout->addWidget(m_userList);
    mainLayout->addWidget(m_privateChatButton);
    mainLayout->addWidget(m_backButton);

    connect(m_privateChatButton, &QPushButton::clicked, this, &UsersWindow::on_privateChatButton_clicked);
    connect(m_backButton, &QPushButton::clicked, this, &UsersWindow::on_backButton_clicked);
}

void UsersWindow::updateUserList(const QStringList& users)
{
    m_userList->clear();
    for (const QString& u : users)
        if (u != m_myUsername)
            m_userList->addItem(u);
    m_hintLabel->setText(m_userList->count() == 0
        ? "No other users online yet"
        : "Select a user to start a private chat");
}

void UsersWindow::on_privateChatButton_clicked()
{
    QListWidgetItem* item = m_userList->currentItem();
    if (!item) { QMessageBox::information(this, "Select User", "Please select a user first."); return; }
    emit privateChatRequested(item->text());
    hide();
}

void UsersWindow::on_backButton_clicked() { hide(); }
