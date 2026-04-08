#include "loginwindow.h"
#include "ui_loginwindow.h"
#include "chat.h"
#include <QString>

LoginWindow::LoginWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::LoginWindow)
{
    ui->setupUi(this);
}

LoginWindow::~LoginWindow()
{
    delete ui;
}

void LoginWindow::on_loginpushButton_clicked()
{
    QString username = ui->usernamelineEdit->text();
    Chat *c = new Chat;
    c->username = username;
    c->show();
    hide();

}

