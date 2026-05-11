#include "settings.h"
#include "ui_settings.h"
#include "ui_changeusername.h"
#include "ui_chat.h"
#include "chat.h"
#include "changeusername.h"
#include "loginwindow.h"
#include <QMessageBox>
#include <QInputDialog>

Settings::Settings(Chat *chatWindow, QWidget *parent)
    : QWidget(parent), ui(new Ui::Settings), chat(chatWindow)
{
    ui->setupUi(this);
}

Settings::~Settings() { delete ui; }

void Settings::on_ChangeUserpushButton_clicked()
{
    Changeusername *c = new Changeusername(chat);
    c->ui->currentusernamelabel->setText(chat->username);
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
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Close Chat?", "Return to Login",
        QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        LoginWindow *l = new LoginWindow;
        l->show();
        hide();
    } else hide();
}

void Settings::on_BackpushButton_clicked()
{
    chat->show();
    hide();
}

void Settings::on_privateMsgpushButton_clicked()
{
    chat->on_userspushButton_clicked();
    chat->show();
    hide();
}

void Settings::on_createGroupButton_clicked()
{
    bool ok;
    QString groupName = QInputDialog::getText(
        this, "Create / Join Group", "Group name:",
        QLineEdit::Normal, "", &ok);
    if (!ok || groupName.trimmed().isEmpty()) return;
    chat->joinGroup(groupName.trimmed());
    chat->show();
    hide();
}

void Settings::on_groupMsgpushButton_clicked()
{
    chat->on_groupspushButton_clicked();
    chat->show();
    hide();
}
