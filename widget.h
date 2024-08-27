#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <qpixmap.h>
#include <QEvent>
#include <qmessagebox.h>
#include <QWidget>
#include <QGraphicsColorizeEffect>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QPainterPath>
#include <QPainter>
#include <QMouseEvent>
#include <QPixMap>
#include <QLabel>
#include <stack>

/* 全局动画时间 */
const int DURATION = 150;

QT_BEGIN_NAMESPACE
namespace Ui {
    class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override; // 添加这一行

private:
    Ui::Widget *ui;
    QLabel *button[6];
    QPixmap pixmap[6][2];
    int switchPixmap[6];
    /* 存储鼠标移动位置 */
    QPoint dragPosition;

    /* 其他页面 */
    QWidget *naviWidget, *browserWidget, *indexWidget, *chatWidget, *mediaWidget, *settingWidget;

    /* 页面栈 */
    std::stack<int> page;

protected:
    /* 无边框窗口相关 */
    void paintEvent(QPaintEvent *event) override; /* 窗口阴影 */
    void mousePressEvent(QMouseEvent *event) override; /* 鼠标按下 */
    void mouseMoveEvent(QMouseEvent *event) override; /* 鼠标移动 */

    /* 页面管理 */
    void pushPage(int index); /* 压入页面 */
    void popPage(); /* 上一页 */

public:
    /* 图片处理相关 */
    QPixmap getPixmapWithBorderRadius(const QPixmap &pixmap, int radius); /* 圆角图片 */
    QPixmap adjustPixmapBrightness(const QPixmap &pixmap, int delta); /* 图片亮度调整 */

    /* 渐变动画相关 */
    void setGradient(QWidget *a, QColor start, QColor end, int duration); /* 背景渐变 */
    void setGradient(QLabel *a, QPixmap pixmap, int duration); /* 图片亮度渐变 */
    void setTabGradient(QLabel *a, int i, int duration); /* 图片亮度渐变 */

private slots:
    void on_pushButton_clicked();
};
#endif // WIDGET_H
