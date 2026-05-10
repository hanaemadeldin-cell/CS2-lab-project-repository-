#include "groupswindow.h"
#include <QTextEdit>
#include <QSplitter>
#include <QMessageBox>
#include <QInputDialog>

GroupsWindow::GroupsWindow(const QString& myUsername, Database* db, QWidget* parent)
    : QWidget(parent)
    , m_myUsername(myUsername)
    , m_db(db)
{
    setWindowTitle("Group Chats");
    resize(700, 500);

    // ── Left panel: group list + controls ─────────────────────────────────────
    auto* leftPanel  = new QWidget;
    auto* leftLayout = new QVBoxLayout(leftPanel);
    leftPanel->setMaximumWidth(200);

    leftLayout->addWidget(new QLabel("<b>My Groups</b>"));
    m_groupList = new QListWidget;
    leftLayout->addWidget(m_groupList);

    m_groupNameEdit = new QLineEdit;
    m_groupNameEdit->setPlaceholderText("Group name...");
    leftLayout->addWidget(m_groupNameEdit);

    m_createButton = new QPushButton("➕ Create Group");
    m_joinButton   = new QPushButton("🔗 Join Group");
    leftLayout->addWidget(m_createButton);
    leftLayout->addWidget(m_joinButton);

    leftLayout->addWidget(new QLabel("<b>Members</b>"));
    m_memberList = new QListWidget;
    m_memberList->setMaximumHeight(120);
    leftLayout->addWidget(m_memberList);

    m_backButton = new QPushButton("Back");
    leftLayout->addWidget(m_backButton);

    // ── Right panel: chat area + message input ────────────────────────────────
    auto* rightPanel  = new QWidget;
    auto* rightLayout = new QVBoxLayout(rightPanel);

    m_currentGroupLabel = new QLabel("Select or create a group");
    m_currentGroupLabel->setStyleSheet("font-weight: bold; color: #333;");
    rightLayout->addWidget(m_currentGroupLabel);

    m_chatArea = new QTextEdit;
    m_chatArea->setReadOnly(true);
    rightLayout->addWidget(m_chatArea);

    auto* inputRow = new QHBoxLayout;
    m_messageEdit = new QLineEdit;
    m_messageEdit->setPlaceholderText("Type a group message...");
    m_sendButton  = new QPushButton("Send");
    m_sendButton->setEnabled(false);
    inputRow->addWidget(m_messageEdit);
    inputRow->addWidget(m_sendButton);
    rightLayout->addLayout(inputRow);

    // ── Splitter layout ───────────────────────────────────────────────────────
    auto* splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(leftPanel);
    splitter->addWidget(rightPanel);
    splitter->setStretchFactor(1, 3);

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(splitter);

    // ── Connections ───────────────────────────────────────────────────────────
    connect(m_createButton, &QPushButton::clicked, this, &GroupsWindow::on_createButton_clicked);
    connect(m_joinButton,   &QPushButton::clicked, this, &GroupsWindow::on_joinButton_clicked);
    connect(m_sendButton,   &QPushButton::clicked, this, &GroupsWindow::on_sendButton_clicked);
    connect(m_backButton,   &QPushButton::clicked, this, &GroupsWindow::on_backButton_clicked);
    connect(m_groupList, &QListWidget::itemClicked, this, &GroupsWindow::on_groupSelected);
    connect(m_messageEdit, &QLineEdit::returnPressed, this, &GroupsWindow::on_sendButton_clicked);

    loadGroups();
}

void GroupsWindow::loadGroups()
{
    m_groupList->clear();
    if (!m_db->isOpen()) return;
    for (const QString& g : m_db->getAllGroups())
        m_groupList->addItem(g);
}

void GroupsWindow::loadGroupHistory(const QString& groupName)
{
    m_chatArea->clear();
    if (!m_db->isOpen()) return;
    auto history = m_db->getGroupHistory(groupName, 100);
    for (const ChatMessage& msg : history)
        m_chatArea->append(msg.timestamp.toString("hh:mm") + " - " +
                           msg.sender + ": " + msg.text);
}

void GroupsWindow::loadGroupMembers(const QString& groupName)
{
    m_memberList->clear();
    if (!m_db->isOpen()) return;
    for (const QString& u : m_db->getGroupMembers(groupName))
        m_memberList->addItem(u);
}

void GroupsWindow::on_createButton_clicked()
{
    QString name = m_groupNameEdit->text().trimmed();
    if (name.isEmpty()) {
        QMessageBox::warning(this, "Error", "Enter a group name first.");
        return;
    }
    if (m_db->isOpen()) {
        m_db->createGroup(name, m_myUsername);
        loadGroups();
    }
    emit joinGroupRequested(name);
    m_currentGroup = name;
    m_currentGroupLabel->setText("Group: #" + name);
    m_sendButton->setEnabled(true);
    loadGroupHistory(name);
    loadGroupMembers(name);
    m_groupNameEdit->clear();
}

void GroupsWindow::on_joinButton_clicked()
{
    QString name = m_groupNameEdit->text().trimmed();
    if (name.isEmpty()) {
        QMessageBox::warning(this, "Error", "Enter a group name to join.");
        return;
    }
    if (m_db->isOpen()) {
        m_db->addUserToGroup(name, m_myUsername);
        loadGroups();
    }
    emit joinGroupRequested(name);
    m_currentGroup = name;
    m_currentGroupLabel->setText("Group: #" + name);
    m_sendButton->setEnabled(true);
    loadGroupHistory(name);
    loadGroupMembers(name);
    m_groupNameEdit->clear();
}

void GroupsWindow::on_sendButton_clicked()
{
    QString text = m_messageEdit->text().trimmed();
    if (text.isEmpty() || m_currentGroup.isEmpty()) return;

    // Save to DB
    if (m_db->isOpen())
        m_db->saveMessage(m_myUsername, m_currentGroup, text, "group");

    // Show locally
    m_chatArea->append(QTime::currentTime().toString("hh:mm") + " - " +
                       m_myUsername + ": " + text);
    m_messageEdit->clear();

    // Ask chat.cpp to send via network
    emit groupMessageRequested(m_currentGroup, text);
}

void GroupsWindow::on_groupSelected(QListWidgetItem* item)
{
    m_currentGroup = item->text();
    m_currentGroupLabel->setText("Group: #" + m_currentGroup);
    m_sendButton->setEnabled(true);
    loadGroupHistory(m_currentGroup);
    loadGroupMembers(m_currentGroup);
}

void GroupsWindow::appendGroupMessage(const QString& groupName,
                                       const QString& sender,
                                       const QString& text)
{
    if (groupName != m_currentGroup) return;  // only show if viewing this group
    m_chatArea->append(QTime::currentTime().toString("hh:mm") + " - " +
                       sender + ": " + text);
}

void GroupsWindow::on_backButton_clicked() { hide(); }
