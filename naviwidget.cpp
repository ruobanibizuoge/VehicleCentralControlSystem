#include "naviwidget.h"
#include "ui_naviwidget.h"

naviWidget::naviWidget(QWidget *parent, Widget *father)
    : QWidget(parent)
    , ui(new Ui::naviWidget)
    , mainWidget(father)
{
    ui->setupUi(this);
}

naviWidget::~naviWidget()
{
    delete ui;
}
