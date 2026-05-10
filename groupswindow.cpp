#include "groupswindow.h"
#include <QMessageBox>

GroupsWindow::GroupsWindow(const QString& myUsername, Database* db, QWidget* parent)
    : QWidget(parent, Qt::Window)
    , m_myUsername(myUsername)
    , m_db(db)
{
    setWindowTitle("Group Chats");
    resize(650, 480);
    setStyleSheet(
        "QWidget { background: #f5f5f5; font-family: -apple-system; font-size: 13px; }"
        "QTextEdit { background: white; border: 1px solid #ddd; border-radius: 8px; padding: 8px; }"
        "QLineEdit { background: white; border: 1px solid #ddd; border-radius: 8px; padding: 6px 10px; }"
        "QListWidget { background: white; border: 1px solid #ddd; border-radius: 8px; padding: 4px; }"
        "QPushButton { background: #0084ff; color: white; border: none; border-radius: 8px; padding: 7px 14px; font-weight: bold; }"
        "QPushButton#m_backButton { background: #e0e0e0; color: #333; }"
    );

    auto* leftPanel  = new QWidget;
    auto* leftLayout = new QVBoxLayout(leftPanel);
    leftPanel->setFixedWidth(180);
    leftLayout->setContentsMargins(0,0,0,0);
    leftLayout->setSpacing(6);
    leftLayout->addWidget(new QLabel("<b>My Groups</b>"));
    m_groupList = new QListWidget;
    leftLayout->addWidget(m_groupList);
    m_groupNameEdit = new QLineEdit;
    m_groupNameEdit->setPlaceholderText("Group name...");
    leftLayout->addWidget(m_groupNameEdit);
    m_createButton = new QPushButton("➕ Create");
    m_joinButton   = new QPushButton("🔗 Join");
    leftLayout->addWidget(m_createButton);
    leftLayout->addWidget(m_joinButton);
    leftLayout->addWidget(new QLabel("<b>Members</b>"));
    m_memberList = new QListWidget;
    m_memberList->setFixedHeight(100);
    leftLayout->addWidget(m_memberList);
    m_backButton = new QPushButton("Close");
    m_backButton->setObjectName("m_backButton");
    leftLayout->addWidget(m_backButton);

    auto* rightPanel  = new QWidget;
    auto* rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(0,0,0,0);
    rightLayout->setSpacing(8);
    m_currentGroupLabel = new QLabel("Select or create a group to start chatting");
    m_currentGroupLabel->setStyleSheet("font-weight: bold; color: #555; padding: 4px;");
    rightLayout->addWidget(m_currentGroupLabel);
    m_chatArea = new QTextEdit;
    m_chatArea->setReadOnly(true);
    rightLayout->addWidget(m_chatArea);
    auto* inputRow = new QHBoxLayout;
    m_messageEdit = new QLineEdit;
    m_messageEdit->setPlaceholderText("Type a group message...");
    m_messageEdit->setFixedHeight(36);
    m_sendButton  = new QPushButton("Send");
    m_sendButton->setFixedSize(70, 36);
    m_sendButton->setEnabled(false);
    inputRow->addWidget(m_messageEdit);
    inputRow->addWidget(m_sendButton);
    rightLayout->addLayout(inputRow);

    auto* splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(leftPanel);
    splitter->addWidget(rightPanel);
    splitter->setStretchFactor(1, 3);
    splitter->setHandleWidth(1);

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(16, 16, 16, 16);
    mainLayout->addWidget(splitter);

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
    for (const QString& g : m_db->getAllGroups()) m_groupList->addItem(g);
}

void GroupsWindow::loadGroupHistory(const QString& groupName)
{
    m_chatArea->clear();
    if (!m_db->isOpen()) return;
    for (const ChatMessage& msg : m_db->getGroupHistory(groupName, 100))
        m_chatArea->append("<span style='color:#888;'>" + msg.timestamp.toString("hh:mm") +
                           "</span> <b>" + msg.sender + "</b>: " + msg.text);
}

void GroupsWindow::loadGroupMembers(const QString& groupName)
{
    m_memberList->clear();
    if (!m_db->isOpen()) return;
    for (const QString& u : m_db->getGroupMembers(groupName)) m_memberList->addItem(u);
}

void GroupsWindow::on_createButton_clicked()
{
    QString name = m_groupNameEdit->text().trimmed();
    if (name.isEmpty()) { QMessageBox::warning(this, "Error", "Enter a group name."); return; }
    if (m_db->isOpen()) { m_db->createGroup(name, m_myUsername); loadGroups(); }
    emit joinGroupRequested(name);
    m_currentGroup = name;
    m_currentGroupLabel->setText("Group: #" + name);
    m_sendButton->setEnabled(true);
    loadGroupHistory(name); loadGroupMembers(name);
    m_groupNameEdit->clear();
}

void GroupsWindow::on_joinButton_clicked()
{
    QString name = m_groupNameEdit->text().trimmed();
    if (name.isEmpty()) { QMessageBox::warning(this, "Error", "Enter a group name."); return; }
    if (m_db->isOpen()) { m_db->addUserToGroup(name, m_myUsername); loadGroups(); }
    emit joinGroupRequested(name);
    m_currentGroup = name;
    m_currentGroupLabel->setText("Group: #" + name);
    m_sendButton->setEnabled(true);
    loadGroupHistory(name); loadGroupMembers(name);
    m_groupNameEdit->clear();
}

void GroupsWindow::on_sendButton_clicked()
{
    QString text = m_messageEdit->text().trimmed();
    if (text.isEmpty() || m_currentGroup.isEmpty()) return;
    if (m_db->isOpen()) m_db->saveMessage(m_myUsername, m_currentGroup, text, "group");
    m_chatArea->append("<span style='color:#888;'>" + QTime::currentTime().toString("hh:mm") +
                       "</span> <b>" + m_myUsername + "</b>: " + text);
    m_messageEdit->clear();
    emit groupMessageRequested(m_currentGroup, text);
}

void GroupsWindow::on_groupSelected(QListWidgetItem* item)
{
    m_currentGroup = item->text();
    m_currentGroupLabel->setText("Group: #" + m_currentGroup);
    m_sendButton->setEnabled(true);
    loadGroupHistory(m_currentGroup); loadGroupMembers(m_currentGroup);
}

void GroupsWindow::appendGroupMessage(const QString& groupName, const QString& sender, const QString& text)
{
    if (groupName != m_currentGroup) return;
    m_chatArea->append("<span style='color:#888;'>" + QTime::currentTime().toString("hh:mm") +
                       "</span> <b>" + sender + "</b>: " + text);
}

void GroupsWindow::on_backButton_clicked() { hide(); }
