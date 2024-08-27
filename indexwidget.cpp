#include "indexwidget.h"
#include "ui_indexwidget.h"

indexWidget::indexWidget(QWidget *parent, Widget *father)
    : QWidget(parent)
    , ui(new Ui::indexWidget)
    , mainWindow(father)
{
    ui->setupUi(this);
}

indexWidget::~indexWidget()
{
    delete ui;
}
