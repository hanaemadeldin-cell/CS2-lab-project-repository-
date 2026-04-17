#include "chat.h"
#include "ui_chat.h"
#include "loginwindow.h"
#include <QString>
#include <QMessageBox>
#include "chatlogic.h"
#include "networkclient.h"


// send later (we'll connect to network in step 2)
Chat::Chat(QWidget *parent)
    : QWidget(parent),
    ui(new Ui::Chat)
{
    ui->setupUi(this);

    client = new RealNetworkClient(this);
    client->connectToServer("127.0.0.1");

    connect(client, &INetworkClient::messageReceived,
            this, [this](QString user, QString text) {
                ui->chattextEdit->insertPlainText(user + ": " + text + "\n");
            });

    connect(client, &INetworkClient::statusUpdated,
            this, [this](QString status) {
                ui->chattextEdit->insertPlainText("[STATUS] " + status + "\n");
            });
}

Chat::~Chat()
{
    delete ui;
}


void Chat::on_SendpushButton_clicked()
{
    ChatLogic logic;

    QString message = ui->messagelineEdit->text();

    if (!logic.validateMessage(message)) {
        QMessageBox::warning(this, "Error", "Message cannot be empty");
        return;
    }

    // ✅ SHOW MESSAGE IN UI (THIS WAS MISSING)
    ui->chattextEdit->insertPlainText("Me: " + message + "\n");

    // send to server
    client->sendMessage(message);

    ui->messagelineEdit->clear();
}


void Chat::on_ClearpushButton_2_clicked()
{
    ui->messagelineEdit->clear();
}


void Chat::on_ClosepushButton_clicked()
{
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Close Chat?", "Return to Login", QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::Yes){
        LoginWindow *l = new LoginWindow;
        l->show();
        hide();
    }
    else hide();
}

