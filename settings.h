#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>
#include <QLineEdit>

namespace Ui { class Settings; }
class Chat;

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
    void on_privateMsgpushButton_clicked();
    void on_createGroupButton_clicked();
    void on_groupMsgpushButton_clicked();

private:
    Ui::Settings *ui;
    Chat *chat;
};

#endif // SETTINGS_H
