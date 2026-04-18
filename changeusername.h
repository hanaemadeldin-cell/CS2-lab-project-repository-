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
     Ui::Changeusername *ui;
     Chat *chat;

private slots:
    void on_SaveUsernamepushButton_clicked();

private:

};

#endif // CHANGEUSERNAME_H
