#ifndef COLORGRADIENT_H
#define COLORGRADIENT_H

#include <QObject>
#include <QPushButton>
#include <QPropertyAnimation>

class ColorGradient : public QObject
{
    Q_OBJECT
    /* Q_PROPERTY宏，实现QPropertyAnimation修改gradient数据 */
    Q_PROPERTY(qreal gradient READ gradient WRITE setGradient NOTIFY gradientChanged)
    QColor start, end, cur;
    qreal k = 0;
public:
    ColorGradient(QColor s = Qt::white, QColor e = Qt::white);
    qreal gradient() const;
    void setGradient(qreal gradient);
    /* 获取插值颜色 */
    QColor getCurrent();
signals:
    void gradientChanged(qreal gradient);
};

#endif // COLORGRADIENT_H
