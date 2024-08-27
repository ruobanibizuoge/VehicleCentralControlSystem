#include "settingwidget.h"
#include "ui_settingwidget.h"

settingWidget::settingWidget(QWidget *parent, Widget *father)
    : QWidget(parent)
    , ui(new Ui::settingWidget)
    , mainWindow(father)
{
    ui->setupUi(this);
}

settingWidget::~settingWidget()
{
    delete ui;
}
