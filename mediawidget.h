#ifndef MEDIAWIDGET_H
#define MEDIAWIDGET_H

#include <QWidget>
#include "widget.h"

namespace Ui {
    class mediaWidget;
}

class mediaWidget : public QWidget
{
    Q_OBJECT

public:
    explicit mediaWidget(QWidget *parent = nullptr, Widget *father = nullptr);
    ~mediaWidget();

private:
    Ui::mediaWidget *ui;
    Widget *mainWindow;
};

#endif // MEDIAWIDGET_H
