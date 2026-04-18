#include "changeusername.h"
#include "ui_changeusername.h"
#include "settings.h"
#include "chat.h"

Changeusername::Changeusername(Chat *chatWindow, QWidget *parent)
    : QWidget(parent),
    ui(new Ui::Changeusername),
    chat(chatWindow)
{
    ui->setupUi(this);
}

Changeusername::~Changeusername()
{
    delete ui;
}

void Changeusername::on_SaveUsernamepushButton_clicked()
{
    QString newUsername = ui->newusernamelineEdit->text();
    chat->username = newUsername;
    chat->show();
    hide();
}

