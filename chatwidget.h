#ifndef CHATWIDGET_H
#define CHATWIDGET_H

#include <QWidget>
#include <QFont>
#include <QScrollBar>
#include <QScrollArea>
#include <QGridLayout>
#include <QTextEdit>
#include <QHostAddress>
#include <QTcpSocket>
#include <QFileDialog>
#include <QStandardPaths>
#include "widget.h"
#include "colorgradient.h"

namespace Ui {
    class chatWidget;
}

class userButton : public QPushButton
{
    Q_OBJECT

public:
    explicit userButton(QWidget *parent, QString userName, QPixmap userProfile);
    ~userButton();
    void setGradient(QColor start, QColor end, int duration);
    QPropertyAnimation *animation;
    bool selected;
    std::vector<std::pair<userButton*,QString>> messages;
    QString name;
    QPixmap profile;
};

class chatWidget : public QWidget
{
    Q_OBJECT

public:
    explicit chatWidget(QWidget *parent = nullptr, Widget *father = nullptr);
    ~chatWidget();
    void sendMessage(QString msg = "");

protected:
    bool eventFilter(QObject *obj, QEvent *event);
    QWidget* getBubble(QPixmap profile, QString str, int type);
    QWidget* myBubble(QString str);
    QWidget* oppositeBubble(userButton *user, QString str);
    void refreshMessage(userButton *newUser);
    std::pair<int, int> getBubbleSize(QString &str);

private slots:
    void on_sendButton_clicked();
    void receiveMessage();
    void on_fileButton_clicked();

private:
    Ui::chatWidget *ui;
    Widget *mainWidget;
    std::vector<userButton*> userList;
    userButton *currentUser;
    QPixmap myProfile;
    const int textOffset = 27;
    QString myName = "User0";
    QTcpSocket *tcpSocket;
    bool transferring = 0, successfullyOpen = 0;
    QString fileName, fileSender;
    qint64 fileSize, fileReceivedSize;
    QFile file;
};

class keyboardEventfilter : public QObject
{
    Q_OBJECT
public:
    keyboardEventfilter(QObject *parent = nullptr, chatWidget *father = nullptr);
protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
private:
    bool pressingShift = false;
    chatWidget *mainWidget;
};

#endif // CHATWIDGET_H
