#include "privatechatwindow.h"

PrivateChatWindow::PrivateChatWindow(const QString& myUsername,
                                      const QString& peerUsername,
                                      Database* db,
                                      QWidget* parent)
    : QWidget(parent)
    , m_myUsername(myUsername)
    , m_peerUsername(peerUsername)
    , m_db(db)
{
    setWindowTitle("Private Chat - " + peerUsername);
    resize(500, 400);

    auto* layout = new QVBoxLayout(this);

    auto* title = new QLabel("<b>Private chat with " + peerUsername + "</b>");
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title);

    m_chatArea = new QTextEdit;
    m_chatArea->setReadOnly(true);
    layout->addWidget(m_chatArea);

    auto* inputRow = new QHBoxLayout;
    m_messageEdit = new QLineEdit;
    m_messageEdit->setPlaceholderText("Type a private message...");
    m_sendButton  = new QPushButton("Send");
    m_backButton  = new QPushButton("Close");
    inputRow->addWidget(m_messageEdit);
    inputRow->addWidget(m_sendButton);
    inputRow->addWidget(m_backButton);
    layout->addLayout(inputRow);

    connect(m_sendButton,  &QPushButton::clicked,   this, &PrivateChatWindow::on_sendButton_clicked);
    connect(m_backButton,  &QPushButton::clicked,   this, &PrivateChatWindow::on_backButton_clicked);
    connect(m_messageEdit, &QLineEdit::returnPressed, this, &PrivateChatWindow::on_sendButton_clicked);

    loadHistory();
}

void PrivateChatWindow::loadHistory()
{
    if (!m_db->isOpen()) return;
    auto history = m_db->getPrivateHistory(m_myUsername, m_peerUsername, 100);
    for (const ChatMessage& msg : history)
        m_chatArea->append(msg.timestamp.toString("hh:mm") + " - " +
                           msg.sender + ": " + msg.text);
}

void PrivateChatWindow::on_sendButton_clicked()
{
    QString text = m_messageEdit->text().trimmed();
    if (text.isEmpty()) return;

    // Save to DB
    if (m_db->isOpen())
        m_db->saveMessage(m_myUsername, m_peerUsername, text, "private");

    // Show locally
    m_chatArea->append(QTime::currentTime().toString("hh:mm") + " - " +
                       m_myUsername + ": " + text);
    m_messageEdit->clear();

    // Ask chat.cpp to send via network
    emit privateMessageRequested(m_peerUsername, text);
}

void PrivateChatWindow::appendIncoming(const QString& sender, const QString& text)
{
    m_chatArea->append(QTime::currentTime().toString("hh:mm") + " - " +
                       sender + ": " + text);
    if (m_db->isOpen())
        m_db->saveMessage(sender, m_myUsername, text, "private");
}

void PrivateChatWindow::on_backButton_clicked() { hide(); }
