#include "mediawidget.h"
#include "ui_mediawidget.h"

mediaWidget::mediaWidget(QWidget *parent, Widget *father)
    : QWidget(parent)
    , ui(new Ui::mediaWidget)
    , mainWindow(father)
{
    ui->setupUi(this);
}

mediaWidget::~mediaWidget()
{
    delete ui;
}
