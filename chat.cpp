#include "chat.h"
#include "ui_chat.h"
#include "loginwindow.h"
#include <QString>
#include <QMessageBox>

Chat::Chat(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Chat)
{
    ui->setupUi(this);
}

Chat::~Chat()
{
    delete ui;
}


void Chat::on_SendpushButton_clicked()
{
    QString message = ui->messagelineEdit->text();
    ui->chattextEdit->insertPlainText(username);
    ui->chattextEdit->insertPlainText(": ");
    ui->chattextEdit->insertPlainText(message);
    ui->chattextEdit->insertPlainText("\n");
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

