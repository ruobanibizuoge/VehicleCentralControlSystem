#include "browserwidget.h"
#include "ui_browserwidget.h"

browserWidget::browserWidget(QWidget *parent, Widget *father)
    : QWidget(parent)
    , ui(new Ui::browserWidget)
    , mainWindow(father)
{
    ui->setupUi(this);
}

browserWidget::~browserWidget()
{
    delete ui;
}
