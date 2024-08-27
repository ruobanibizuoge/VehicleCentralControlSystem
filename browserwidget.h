#ifndef BROWSERWIDGET_H
#define BROWSERWIDGET_H

#include <QWidget>
#include "widget.h"

namespace Ui {
    class browserWidget;
}

class browserWidget : public QWidget
{
    Q_OBJECT

public:
    explicit browserWidget(QWidget *parent = nullptr, Widget *father = nullptr);
    ~browserWidget();

private:
    Ui::browserWidget *ui;
    Widget *mainWindow;
};

#endif // BROWSERWIDGET_H
