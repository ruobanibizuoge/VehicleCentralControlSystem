#include "hovereventfilter.h"

HoverEventfilter::HoverEventfilter(QObject *parent, QPropertyAnimation *ani) :
    QObject(parent), animation(ani) {}

bool HoverEventfilter::eventFilter(QObject *obj, QEvent *event)
{
    QWidget *element = qobject_cast<QWidget *>(obj);
    if (element && event->type() == QEvent::Enter)
    {
        /* 鼠标进入，正向动画 */
        animation->setDirection(QPropertyAnimation::Forward);
        animation->start();
        return true;
    } else if (element && event->type() == QEvent::Leave)
    {
        /* 鼠标离开，逆向动画 */
        animation->setDirection(QPropertyAnimation::Backward);
        animation->start();
        return true;
    }
    return QObject::eventFilter(obj, event);
}
