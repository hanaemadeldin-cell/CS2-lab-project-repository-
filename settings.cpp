#include "settings.h"
#include "loginwindow.h"
#include "ui_chat.h"
#include "ui_settings.h"
#include "changeusername.h"
#include "chat.h"
#include <QtWidgets/qmessagebox.h>

Settings::Settings(Chat *chatWindow, QWidget *parent)
    : QWidget(parent),
    ui(new Ui::Settings),
    chat(chatWindow)
{
    ui->setupUi(this);
}


Settings::~Settings()
{
    delete ui;
}

void Settings::on_ChangeUserpushButton_clicked()
{
    Changeusername *c = new Changeusername(chat);
    c->show();
    hide();

}


void Settings::on_clearchatpushButton_clicked()
{
    chat->ui->chattextEdit->clear();
    chat->show();
    hide();
}


void Settings::on_LogoutpushButton_clicked()
{
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Close Chat?", "Return to Login", QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::Yes){
        LoginWindow *l = new LoginWindow;
        l->show();
        hide();
    }
    else hide();
}


void Settings::on_BackpushButton_clicked()
{
    chat->show();
    hide();
}

