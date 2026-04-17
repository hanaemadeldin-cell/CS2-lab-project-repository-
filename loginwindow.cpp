#include "loginwindow.h"
#include "ui_loginwindow.h"
#include "chat.h"
#include <QString>
#include "chatlogic.h"
#include <QMessageBox>

void LoginWindow::on_loginpushButton_clicked()
{
    ChatLogic logic;

    QString username = ui->usernamelineEdit->text();

    if (!logic.validateUsername(username)) {
        QMessageBox::warning(this, "Error", "Username cannot be empty");
        return;
    }

   Chat *c = new Chat(username);
    c->show();
    hide();
}
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



