#ifndef SETTINGWIDGET_H
#define SETTINGWIDGET_H

#include <QWidget>
#include "widget.h"

namespace Ui {
class settingWidget;
}

class settingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit settingWidget(QWidget *parent = nullptr, Widget *father = nullptr);
    ~settingWidget();

private:
    Ui::settingWidget *ui;
    Widget *mainWindow;
};

#endif // SETTINGWIDGET_H
