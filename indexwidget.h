#ifndef INDEXWIDGET_H
#define INDEXWIDGET_H

#include <QWidget>
#include "widget.h"
#include <weatherdata.h>
#include <QCompleter>

#include <QtCharts>

#include <QChartView>
#include <QNetworkAccessManager>
#include <QSplineSeries>


namespace Ui {
    class indexWidget;
}

class indexWidget : public QWidget
{
    Q_OBJECT

public:
    explicit indexWidget(QWidget *parent = nullptr, Widget *father = nullptr);
    ~indexWidget();

    //搜索框输入建议
    QCompleter *completer;

    //数据
    QList<weatherdata> datalist;
    //aqi图
    QLineSeries *aqi_series;
    QChart *aqi_chart;
    QChartView *chartView;
    void draw_aqi();

    //自动获取当前城市
    bool flag;//判断是否是当地信息
    QString ip;
    QNetworkAccessManager *m_ipManager;
    void get_ip();
    void init_ip();
    QString get_loc();


    //模糊搜索城市名称
    QString search_cityname();

    //获取城市代码方法
    QString get_citycode();

    //解析天气数据方法
    void parse_weatherdata();

    //解析日期方法
    QString parse_date(QString pre_date);

    //初始化方法
    void suggest();

    //填充标签方法
    void fill_lab();

    //预设图片
    void init_pic();

    //获取天气类型图片
    QString get_typepic(QString type);

    /* 渐变动画相关 */
    QPixmap adjustPixmapBrightness(const QPixmap &pixmap, int delta);
    void setGradient(QWidget *a, QColor start, QColor end, int duration); /* 背景渐变 */
    void setGradient(QLabel *a, QPixmap pixmap, int duration); /* 图片亮度渐变 */

signals:

private slots:
    //自动填充
    void onQueryClicked();
    void onGetIp(QNetworkReply*);


    void on_location_clicked();

private:
    Ui::indexWidget *ui;
    Widget *mainWindow;
};

#endif // INDEXWIDGET_H
