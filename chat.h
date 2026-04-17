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
    explicit Chat(QWidget *parent = nullptr);
    ~Chat();
    QString username;

private slots:
    void on_SendpushButton_clicked();
    void on_ClearpushButton_2_clicked();
    void on_ClosepushButton_clicked();

private:
    Ui::Chat *ui;

  INetworkClient* client;
};

#endif // CHAT_H
