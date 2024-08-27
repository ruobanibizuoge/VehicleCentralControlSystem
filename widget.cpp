#include "widget.h"
#include "ui_widget.h"
#include "hovereventfilter.h"
#include "colorgradient.h"
#include "multiWidget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    setGradient(ui->pushButton, QColor(72, 72, 84), Qt::red, DURATION);
    setGradient(ui->backButton, QPixmap(":/icons/back.png"), DURATION);

    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect;
    shadowEffect->setOffset(0, 0);
    shadowEffect->setBlurRadius(15);
    shadowEffect->setColor(Qt::black);
    this->setGraphicsEffect(shadowEffect);

    pixmap[0][0]=QPixmap(":/icons/navi.png");
    pixmap[0][1]=QPixmap(":/icons/navi1.png");
    pixmap[1][0]=QPixmap(":/icons/browser.png");
    pixmap[1][1]=QPixmap(":/icons/browser1.png");
    pixmap[2][0]=QPixmap(":/icons/index.png");
    pixmap[2][1]=QPixmap(":/icons/index1.png");
    pixmap[3][0]=QPixmap(":/icons/chat.png");
    pixmap[3][1]=QPixmap(":/icons/chat1.png");
    pixmap[4][0]=QPixmap(":/icons/media.png");
    pixmap[4][1]=QPixmap(":/icons/media1.png");
    pixmap[5][0]=QPixmap(":/icons/setting.png");
    pixmap[5][1]=QPixmap(":/icons/setting1.png");

    button[0]=ui->NaviButton;
    button[1]=ui->BrowserButton;
    button[2]=ui->IndexButton;
    button[3]=ui->ChatButton;
    button[4]=ui->MediaButton;
    button[5]=ui->SettingButton;

    for (int i = 0; i < 6 ; i++)
    {
        pixmap[i][0] = getPixmapWithBorderRadius(pixmap[i][0].scaled(QSize(80, 80), Qt::KeepAspectRatio, Qt::SmoothTransformation), 10);
        pixmap[i][1] = getPixmapWithBorderRadius(pixmap[i][1].scaled(QSize(80, 80), Qt::KeepAspectRatio, Qt::SmoothTransformation), 10);
        button[i]->setPixmap(pixmap[i][0]);
        switchPixmap[i] = 0;
        button[i]->installEventFilter(this);
        setTabGradient(button[i], i, DURATION);
    }

    ui->backButton->installEventFilter(this);

    naviWidget = new class naviWidget(this, this);
    browserWidget = new class browserWidget(this, this);
    indexWidget = new class indexWidget(this, this);
    chatWidget = new class chatWidget(this, this);
    mediaWidget = new class mediaWidget(this, this);
    settingWidget = new class settingWidget(this, this);

    ui->mainWidget->addWidget(naviWidget);
    ui->mainWidget->addWidget(browserWidget);
    ui->mainWidget->addWidget(indexWidget);
    ui->mainWidget->addWidget(chatWidget);
    ui->mainWidget->addWidget(mediaWidget);
    ui->mainWidget->addWidget(settingWidget);

    pushPage(3);
    button[3]->setPixmap(pixmap[3][1]);

}

Widget::~Widget()
{
    delete ui;
}

// TabBuilder控制器
bool Widget::eventFilter(QObject *obj, QEvent *event)
{
    /*
     * 0:Navi,
     * 1:Browser,
     * 2:Index,
     * 3:Chat,
     * 4:Media,
     * 5:Setting
     */
    QLabel *cur = qobject_cast<QLabel*>(obj);
    if (cur && event->type() == QEvent::MouseButtonRelease)
    {
        if (cur == ui->backButton)
        {
            popPage();
            return true;
        } else
        {
            for (int i = 0; i < 6; i++)
                if (button[i] == cur)
                {
                    pushPage(i);
                    return true;
                }
        }
    }
    return QObject::eventFilter(obj, event);
}

void Widget::paintEvent(QPaintEvent *event)
{
    /* 圆角窗口 */
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QBrush(QColor(36, 36, 42)));
    painter.setPen(Qt::NoPen);
    QRect rect = ui->mainWidget->rect();
    rect.setRect(ui->mainWidget->pos().x(), ui->mainWidget->pos().y(), rect.width(), rect.height());
    painter.drawRoundedRect(rect, 10, 10);
    rect.setRect(ui->mainWidget->pos().x(), ui->mainWidget->pos().y(), 10, rect.height());
    painter.drawRect(rect);
    painter.setBrush(QBrush(QColor(0, 0, 0)));
    rect = ui->tabWidget->rect();
    rect.setRect(ui->tabWidget->pos().x(), ui->tabWidget->pos().y(), rect.width(), rect.height());
    painter.drawRoundedRect(rect, 10, 10);
    rect.setRect(ui->tabWidget->pos().x() + rect.width() - 10, ui->tabWidget->pos().y(), 10, rect.height());
    painter.drawRect(rect);
    QWidget::paintEvent(event);
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    dragPosition=event->globalPos()-frameGeometry().topLeft();

}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    if (dragPosition.x() >= 130 && dragPosition.y() <= 110)
        move(event->globalPos()-dragPosition);
}

QPixmap Widget::getPixmapWithBorderRadius(const QPixmap &pixmap, int radius)
{
    /* 用蒙版裁切图片实现圆角 */
    QPixmap resultPixmap = pixmap;
    QBitmap mask(pixmap.size());
    QPainter painter(&mask);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(mask.rect(), Qt::color0);
    painter.setBrush(Qt::color1);
    painter.drawRoundedRect(mask.rect(),radius,radius);
    resultPixmap.setMask(mask);
    return resultPixmap;
}

QPixmap Widget::adjustPixmapBrightness(const QPixmap &pixmap, int delta)
{
    /* 将QPixmap转成QImage再修改每个像素点的颜色 */
    QImage image = pixmap.toImage();
    image = image.convertToFormat(QImage::Format_ARGB32);
    for (int y = 0; y < image.height(); y++)
        for (int x = 0; x < image.width(); x++)
        {
            QColor color = image.pixelColor(x, y);
            color.setRed(qMin(qMax(color.red() + delta, 0), 255));
            color.setGreen(qMin(qMax(color.green() + delta, 0), 255));
            color.setBlue(qMin(qMax(color.blue() + delta, 0), 255));
            image.setPixelColor(x, y, color);
        }
    return QPixmap::fromImage(image);
}

void Widget::setGradient(QWidget *a, QColor start, QColor end, int duration)
{
    /* 渐变颜色 */
    ColorGradient *color = new ColorGradient(start, end);
    /* 设置动画 */
    QPropertyAnimation *animation = new QPropertyAnimation(color, "gradient");
    animation->setDuration(duration);
    animation->setStartValue(0);
    animation->setEndValue(1);
    /* 设置hover的事件过滤器 */
    HoverEventfilter* filter = new HoverEventfilter(this, animation);
    a->installEventFilter(filter);
    /* 将渐变值更改的信号链接上修改颜色的槽 */
    QObject::connect(color, &ColorGradient::gradientChanged, a,
                     [a, color]
                     {
                         a->setStyleSheet("background-color:rgba("+
                                          QString::number(color->getCurrent().red())+","+
                                          QString::number(color->getCurrent().green())+","+
                                          QString::number(color->getCurrent().blue())+","+
                                          QString::number(color->getCurrent().alpha())+");");
                     });
}

void Widget::setGradient(QLabel *a, QPixmap profile, int duration)
{
    ColorGradient *color = new ColorGradient();
    QPropertyAnimation *animation = new QPropertyAnimation(color, "gradient");
    animation->setDuration(duration);
    animation->setStartValue(0);
    animation->setEndValue(1);
    HoverEventfilter* filter = new HoverEventfilter(this, animation);
    a->installEventFilter(filter);
    /* 用图片亮度变化实现图片渐变 */
    QObject::connect(color, &ColorGradient::gradientChanged, a,
                     [this, a, color, profile]
                     {
                         QPixmap pic = adjustPixmapBrightness(profile, (int)(color->gradient() * 150));
                         a->setPixmap(pic);
                     });
}

void Widget::setTabGradient(QLabel *a, int i, int duration)
{
    ColorGradient *color = new ColorGradient();
    QPropertyAnimation *animation = new QPropertyAnimation(color, "gradient");
    animation->setDuration(duration);
    animation->setStartValue(0);
    animation->setEndValue(1);
    HoverEventfilter* filter = new HoverEventfilter(this, animation);
    a->installEventFilter(filter);
    /* 用图片亮度变化实现图片渐变 */
    QObject::connect(color, &ColorGradient::gradientChanged, a,
                     [this, a, color, i]
                     {
                         QPixmap pic = adjustPixmapBrightness(pixmap[i][switchPixmap[i]], (int)(color->gradient() * 150));
                         a->setPixmap(pic);
                     });
}

/* 页面管理 */
void Widget::pushPage(int index)
{
    ui->mainWidget->setCurrentIndex(index);
    for (int i = 0; i < 6; i++)
        if (i == index)
        {
            switchPixmap[i] = 1;
        } else
        {
            button[i]->setPixmap(pixmap[i][0]);
            switchPixmap[i] = 0;
        }
    page.push(index);
}

void Widget::popPage()
{
    if (page.size() == 1) return;
    page.pop();
    int currentPage = page.top();
    ui->mainWidget->setCurrentIndex(currentPage);
    for (int i = 0; i < 6; i++)
        if (i == currentPage)
        {
            button[i]->setPixmap(pixmap[i][1]);
            switchPixmap[i] = 1;
        } else
        {
            button[i]->setPixmap(pixmap[i][0]);
            switchPixmap[i] = 0;
        }
}

void Widget::on_pushButton_clicked()
{
    exit(0);
}

