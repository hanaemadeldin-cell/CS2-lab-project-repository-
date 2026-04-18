#ifndef CHAT_H
#define CHAT_H

#include <QWidget>
#include "networkclient.h"


namespace Ui {
class Chat;
}

class Chat : public QWidget
{
    Q_OBJECT

public:
    Chat(QString username, QWidget *parent = nullptr);
    ~Chat();
    QString username;
    Ui::Chat *ui;
    Chat *chat;
private slots:
    void on_SendpushButton_clicked();
    void on_ClearpushButton_2_clicked();
    void on_settingspushButton_clicked();

private:

  INetworkClient* client;


};

#endif // CHAT_H
