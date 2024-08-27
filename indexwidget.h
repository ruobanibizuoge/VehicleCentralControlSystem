#ifndef INDEXWIDGET_H
#define INDEXWIDGET_H

#include <QWidget>
#include "widget.h"

namespace Ui {
    class indexWidget;
}

class indexWidget : public QWidget
{
    Q_OBJECT

public:
    explicit indexWidget(QWidget *parent = nullptr, Widget *father = nullptr);
    ~indexWidget();

private:
    Ui::indexWidget *ui;
    Widget *mainWindow;
};

#endif // INDEXWIDGET_H
