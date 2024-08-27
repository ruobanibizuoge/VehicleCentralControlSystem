#include "colorgradient.h"

ColorGradient::ColorGradient(QColor s, QColor e) :
    start(s), end(e) {}

qreal ColorGradient::gradient() const
{
    return k;
}

void ColorGradient::setGradient(qreal gradient)
{
    /* 对于渐变值k计算颜色的线性插值 */
    k = gradient;
    cur.setRed(start.red() + k * (end.red() - start.red()));
    cur.setGreen(start.green() + k * (end.green() - start.green()));
    cur.setBlue(start.blue() + k * (end.blue() - start.blue()));
    cur.setAlpha(start.alpha() + k * (end.alpha() - start.alpha()));
    /* 手动发送渐变值更改的信号（Q_PROPERTY中设置了NOTIFY但实际上并不会自动触发） */
    emit gradientChanged(k);
}

QColor ColorGradient::getCurrent()
{
    return cur;
}
