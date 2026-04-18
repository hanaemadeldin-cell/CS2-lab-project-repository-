#ifndef CHANGEUSERNAME_H
#define CHANGEUSERNAME_H

#include "chat.h"
#include <QWidget>

namespace Ui {
class Changeusername;
}

class Changeusername : public QWidget
{
    Q_OBJECT

public:
     explicit Changeusername(Chat *chatWindow, QWidget *parent = nullptr);
    ~Changeusername();

private slots:
    void on_SaveUsernamepushButton_clicked();

private:
    Ui::Changeusername *ui;
    Chat *chat;
};

#endif // CHANGEUSERNAME_H
