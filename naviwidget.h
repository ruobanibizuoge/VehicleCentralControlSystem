#ifndef NAVIWIDGET_H
#define NAVIWIDGET_H

#include <QWidget>
#include "widget.h"

namespace Ui {
    class naviWidget;
}

class naviWidget : public QWidget
{
    Q_OBJECT

public:
    explicit naviWidget(QWidget *parent = nullptr, Widget *father = nullptr);
    ~naviWidget();

private:
    Ui::naviWidget *ui;
    Widget *mainWidget;
};

#endif // NAVIWIDGET_H
