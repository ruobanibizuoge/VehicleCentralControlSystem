#ifndef HOVEREVENTFILTER_H
#define HOVEREVENTFILTER_H

#include <QObject>
#include <QPushButton>
#include <QEvent>
#include <QPropertyAnimation>

/* 判断是否hover的事件过滤器 */
class HoverEventfilter : public QObject
{
    Q_OBJECT
    QPropertyAnimation *animation;
public:
    HoverEventfilter(QObject *parent = nullptr, QPropertyAnimation *ani = nullptr);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
};

#endif // HOVEREVENTFILTER_H
