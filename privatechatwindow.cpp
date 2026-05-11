#include "privatechatwindow.h"

PrivateChatWindow::PrivateChatWindow(const QString& myUsername,
                                      const QString& peerUsername,
                                      Database* db,
                                      QWidget* parent)
    : QWidget(parent, Qt::Window)
    , m_myUsername(myUsername)
    , m_peerUsername(peerUsername)
    , m_db(db)
{
    setWindowTitle("Private Chat - " + peerUsername);
    setFixedSize(420, 380);
    setStyleSheet(
        "QWidget { background: #f5f5f5; font-family: -apple-system; font-size: 13px; }"
        "QTextEdit { background: white; border: 1px solid #ddd; border-radius: 8px; padding: 8px; }"
        "QLineEdit { background: white; border: 1px solid #ddd; border-radius: 8px; padding: 6px 10px; }"
        "QPushButton { background: #0084ff; color: white; border: none; border-radius: 8px; padding: 7px 16px; font-weight: bold; }"
        "QPushButton#closeBtn { background: #e0e0e0; color: #333; }"
    );

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(10);

    auto* title = new QLabel("<b>Private chat with " + peerUsername + "</b>");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 15px;");
    layout->addWidget(title);

    m_chatArea = new QTextEdit;
    m_chatArea->setReadOnly(true);
    layout->addWidget(m_chatArea);

    auto* inputRow = new QHBoxLayout;
    m_messageEdit = new QLineEdit;
    m_messageEdit->setPlaceholderText("Type a private message...");
    m_messageEdit->setFixedHeight(36);
    m_sendButton = new QPushButton("Send");
    m_sendButton->setFixedSize(70, 36);
    m_backButton = new QPushButton("Close");
    m_backButton->setObjectName("closeBtn");
    m_backButton->setFixedSize(70, 36);
    inputRow->addWidget(m_messageEdit);
    inputRow->addWidget(m_sendButton);
    inputRow->addWidget(m_backButton);
    layout->addLayout(inputRow);

    connect(m_sendButton,  &QPushButton::clicked,     this, &PrivateChatWindow::on_sendButton_clicked);
    connect(m_backButton,  &QPushButton::clicked,     this, &PrivateChatWindow::on_backButton_clicked);
    connect(m_messageEdit, &QLineEdit::returnPressed, this, &PrivateChatWindow::on_sendButton_clicked);

    loadHistory();
}

void PrivateChatWindow::loadHistory()
{
    if (!m_db->isOpen()) return;
    for (const ChatMessage& msg : m_db->getPrivateHistory(m_myUsername, m_peerUsername, 100))
        m_chatArea->append("<span style='color:#888;'>" + msg.timestamp.toString("hh:mm") +
                           "</span> <b>" + msg.sender + "</b>: " + msg.text);
}

void PrivateChatWindow::on_sendButton_clicked()
{
    QString text = m_messageEdit->text().trimmed();
    if (text.isEmpty()) return;
    if (m_db->isOpen())
        m_db->saveMessage(m_myUsername, m_peerUsername, text, "private");
    m_chatArea->append("<span style='color:#888;'>" + QTime::currentTime().toString("hh:mm") +
                       "</span> <b>" + m_myUsername + "</b>: " + text);
    m_messageEdit->clear();
    emit privateMessageRequested(m_peerUsername, text);
}

void PrivateChatWindow::appendIncoming(const QString& sender, const QString& text)
{
    m_chatArea->append("<span style='color:#888;'>" + QTime::currentTime().toString("hh:mm") +
                       "</span> <b>" + sender + "</b>: " + text);
    if (m_db->isOpen())
        m_db->saveMessage(sender, m_myUsername, text, "private");
}

void PrivateChatWindow::on_backButton_clicked() { hide(); }
