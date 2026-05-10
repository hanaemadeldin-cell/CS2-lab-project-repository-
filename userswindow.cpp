#include "userswindow.h"
#include <QMessageBox>

UsersWindow::UsersWindow(const QString& myUsername, QWidget* parent)
    : QWidget(parent)
    , m_myUsername(myUsername)
{
    setWindowTitle("Online Users");
    resize(350, 450);

    m_titleLabel       = new QLabel("<b>Online Users</b>");
    m_titleLabel->setAlignment(Qt::AlignCenter);

    m_hintLabel        = new QLabel("Select a user and click Private Chat");
    m_hintLabel->setAlignment(Qt::AlignCenter);
    m_hintLabel->setStyleSheet("color: gray; font-size: 11px;");

    m_userList         = new QListWidget;
    m_privateChatButton = new QPushButton("💬 Private Chat");
    m_backButton       = new QPushButton("Back");

    auto* btnLayout = new QHBoxLayout;
    btnLayout->addWidget(m_privateChatButton);
    btnLayout->addWidget(m_backButton);

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_titleLabel);
    mainLayout->addWidget(m_hintLabel);
    mainLayout->addWidget(m_userList);
    mainLayout->addLayout(btnLayout);

    connect(m_privateChatButton, &QPushButton::clicked,
            this, &UsersWindow::on_privateChatButton_clicked);
    connect(m_backButton, &QPushButton::clicked,
            this, &UsersWindow::on_backButton_clicked);
}

void UsersWindow::updateUserList(const QStringList& users)
{
    m_userList->clear();
    for (const QString& u : users) {
        if (u != m_myUsername)   // don't show yourself
            m_userList->addItem(u);
    }
}

void UsersWindow::on_privateChatButton_clicked()
{
    QListWidgetItem* item = m_userList->currentItem();
    if (!item) {
        QMessageBox::information(this, "Select User", "Please select a user first.");
        return;
    }
    emit privateChatRequested(item->text());
    hide();
}

void UsersWindow::on_backButton_clicked()
{
    hide();
}
