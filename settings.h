#ifndef SETTINGS_H
#define SETTINGS_H

#include "chat.h"
#include <QWidget>

namespace Ui {
class Settings;
}

class Settings : public QWidget
{
    Q_OBJECT

public:
    explicit Settings(Chat *chatWindow, QWidget *parent = nullptr);
    ~Settings();

private slots:
    void on_ChangeUserpushButton_clicked();

    void on_clearchatpushButton_clicked();

    void on_LogoutpushButton_clicked();

    void on_BackpushButton_clicked();

private:
    Ui::Settings *ui;
    Chat *chat;
};

#endif // SETTINGS_H
