#include "indexwidget.h"
#include "ui_indexwidget.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include <QRegularExpression>
#include "hovereventfilter.h"
#include "colorgradient.h"
#include <qtimer.h>
#include <QRegularExpression>
#include <QCompleter>
#include <QAbstractItemView>
#include <QChart>
#include <QDateTimeAxis>
#include <QRandomGenerator>

indexWidget::indexWidget(QWidget *parent, Widget *father)
    : QWidget(parent)
    , ui(new Ui::indexWidget)
    , mainWindow(father)
{
    ui->setupUi(this);
    get_loc();
    aqi_series = new QLineSeries();
    aqi_chart = new QChart();
    chartView = new QChartView(ui->chartwidget);

    QDateTimeAxis *axisX = new QDateTimeAxis();
    QValueAxis *axisY = new QValueAxis;
    axisX->setFormat("MM/dd"); // 设置日期时间格式
    axisX->setRange(QDateTime::currentDateTime(), QDateTime::currentDateTime().addDays(14));
    axisY->setRange(0, 200);
    axisY->setLabelFormat("%.1f");
    aqi_chart->addAxis(axisX, Qt::AlignBottom); // 将X轴绑定到系列
    aqi_chart->addAxis(axisY, Qt::AlignLeft);
    aqi_chart->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    // aqi_chart->setAxisX(axisX, aqi_series);

    aqi_chart->setTitle("空气质量指数AQI");
    aqi_chart->addSeries(aqi_series);
    aqi_series->attachAxis(axisX);
    aqi_series->attachAxis(axisY);
    aqi_chart->legend()->hide();
    aqi_chart->setTheme(QChart::ChartThemeDark);

    ui->searchbox->setPlaceholderText("请输入需要查询的城市名称...");
    //连接到搜索框填充，onQueryClicked方法最后调用parse_weatherdata
    connect(ui->querybutton, &QPushButton::clicked, this, &indexWidget::onQueryClicked);
    connect(ui->searchbox, &QLineEdit::returnPressed, this, &indexWidget::onQueryClicked);
    connect(ui->querybutton, &QPushButton::clicked, this, &indexWidget::parse_weatherdata);
    connect(ui->searchbox, &QLineEdit::returnPressed, this, &indexWidget::parse_weatherdata);


    setGradient(ui->querybutton, QColor(72, 72, 84), QColorConstants::Svg::lightblue, DURATION);
    setGradient(ui->searchbox, QColor(72, 72, 84), QColorConstants::Svg::darkgrey, DURATION);

    init_pic();
    suggest();
}

indexWidget::~indexWidget()
{
    delete ui;
}

QString indexWidget::get_loc() {
    QString ak = "CDE9axcE8tPnZZRLiPdgK8IIsXOJ77a3";
    QString url = "https://api.map.baidu.com/place/v2/search?query=ATM%E6%9C%BA&tag=%E9%93%B6%E8%A1%8C&region=%E5%8C%97%E4%BA%AC&output=json&ak="+ak;


    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    QNetworkReply *reply = manager->get(request);

    connect(reply, &QNetworkReply::readyRead, [=](){
        QByteArray responseData = reply->readAll();
        QJsonParseError json_error;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData, &json_error);
        if (json_error.error != QJsonParseError::NoError) {
            qWarning() << "JSON Parse Error:" << json_error.errorString();
            reply->deleteLater();
            return;
        }

        if (!jsonDoc.isObject()) {
            qWarning() << "JSON document is not an object";
            reply->deleteLater();
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();
        int status = jsonObj["status"].toInt();
        if (status!=0) {
            qWarning() << status <<"!!!!!!!!!";
            return;
        }

        QJsonObject result_json = jsonObj["result"].toObject();

        QString province_name = result_json["province"].toString();
        qDebug() << province_name;
    });

    return QString();
}

QString indexWidget::get_typepic(QString type) {
    if (type == "晴") return ":/pic/type/qing.png";
    if (type == "多云") return ":/pic/type/duoyun.png";
    if (type == "阴") return ":/pic/type/yin.png";
    if (type == "大雨") return ":/pic/type/dayu.png";
    if (type == "中雨") return ":/pic/type/zhongyu.png";
    if (type == "小雨") return ":/pic/type/xiaoyu.png";
    if (type == "雷阵雨") return ":/pic/type/leizhenyu.png";
    if (type == "小雪") return ":/pic/type/xiaoxue.png";
    if (type == "中雪") return ":/pic/type/zhongxue.png";
    if (type == "大雪") return ":/pic/type/daxue.png";
    if (type == "雨夹雪") return ":/pic/type/yujiaxue.png";
    if (type.endsWith("雨")) return ":/pic/type/dayu.png";
    if (type.endsWith("雪")) return ":/pic/type/daxue.png";
    if (type.contains("雾")) return ":/pic/type/wu.png";
    return ":/pic/type/yin.png";
}

void indexWidget::init_pic() {
    QPixmap pixmap(":/pic/wind.png");
    ui->wind_icon->setPixmap(pixmap);

    pixmap.load(":/pic/pm25.png");
    ui->pm25_icon->setPixmap(pixmap);

    pixmap.load(":/pic/wet.png");
    ui->wet_icon->setPixmap(pixmap);

    pixmap.load(":/pic/quality.png");
    ui->quality_icon->setPixmap(pixmap);

    pixmap.load(":/pic/type/yin.png");
    ui->day1_pic->setPixmap(pixmap);
    ui->day2_pic->setPixmap(pixmap);
    ui->day3_pic->setPixmap(pixmap);
    ui->day4_pic->setPixmap(pixmap);
    ui->day5_pic->setPixmap(pixmap);
    ui->day6_pic->setPixmap(pixmap);
    ui->day7_pic->setPixmap(pixmap);

    pixmap.load(":/pic/weizhi.png");
    ui->weizhi_label->setPixmap(pixmap);

    pixmap.load(":/icons/search.png");
    ui->querybutton->setIcon(pixmap);

    aqi_series->clear();
    QRandomGenerator rng;//生成随机数对象
    for (int i = 0; i < 15; i++) {
        QDateTime d = QDateTime::currentDateTime().addDays(i);
        aqi_series->append(d.toMSecsSinceEpoch(), rng.bounded(25,75));
    }
    aqi_chart->update();
    chartView->setChart(aqi_chart);//向chartView中添加图标
    chartView->resize(QSize(ui->chartwidget->size()));//重新设置chartView的大小
    chartView->setRenderHints(QPainter::Antialiasing);//消除边缘
    chartView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    chartView->show();
    aqi_chart->update();
}

void indexWidget::onQueryClicked() {
    QAbstractItemModel *model = completer->completionModel();
    if (!model) {
        return; // 如果模型为空，则返回
    }

    // 获取模型中的数据
    QStringList completions;
    for (int i = 0; i < model->rowCount(); ++i) {
        QModelIndex index = model->index(i, 0);
        completions << model->data(index).toString();
    }

    if (completions.isEmpty()) {
        return;
        // parse_weatherdata(); // 如果没有建议，不做任何操作
    }

    // 自动补全
    QString text = ui->searchbox->text();
    if (text.isEmpty() || text != completions.first()) {
        ui->searchbox->setText(completions.first());
        ui->searchbox->setCursorPosition(ui->searchbox->text().length()); // 将光标定位到文本末尾
    }


}

//获取搜索建议
void indexWidget::suggest() {
    parse_weatherdata();

    // 创建一个 QStringList 作为建议列表
    QStringList suggestions;
    // suggestions << "Apple" << "Banana" << "Cherry" << "Date" << "Grapes" << "Orange";
    QFile cityname_file(":/cityname/cityname.txt");
    if (!cityname_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "无法打开文件:123" << cityname_file.errorString();
        return;
    }
    QTextStream in(&cityname_file);

    while (!in.atEnd()) {
        suggestions.append(in.readLine().trimmed());
    }
    // 创建并配置 QCompleter
    completer = new QCompleter(suggestions, ui->searchbox);
    completer->setCaseSensitivity(Qt::CaseInsensitive); // 设置为不区分大小写
    completer->setCompletionMode(QCompleter::PopupCompletion); // 以弹出框的方式显示建议
    connect(completer, QOverload<const QString &>::of(&QCompleter::activated),
            this, &indexWidget::parse_weatherdata);

    // 获取 QCompleter 的弹出窗口（下拉列表）
    QAbstractItemView *popup = completer->popup();
    // 设置样式表 (QSS)
    popup->setStyleSheet(
        "QListView {"
        "   background-color: rgb(31, 39, 53);"   // 背景颜色
        "   border: 1px solid gray;"    // 边框
        "   padding: 5px;"              // 内边距
        "   color:white;"
        "   font-size:15px;"
        "}"
        "QListView::item {"
        "   padding: 5px;"              // 每个建议项的内边距
        "   color: white"
        "}"
        "QListView::item:selected {"
        "   background-color: lightblue;"  // 选中项的背景颜色
        "   color: white;"                 // 选中项的文本颜色
        "}"
        "QListView::item:hover {"
        "   background-color: lightgree;"  // 选中项的背景颜色
        "   color: white;"                 // 选中项的文本颜色
        "}"
        );

    ui->searchbox->setCompleter(completer);
}

void indexWidget::fill_lab() {
    if (datalist.isEmpty()) {
        qDebug() << "没有内容";
        return;
    }
    //今天
    weatherdata weather_data = datalist[0];
    ui->wind_dir_lab->setText(weather_data.wind_direction);
    ui->wind_grade->setText(weather_data.wind_grade);
    ui->pm25_lab->setText(weather_data.pm25);
    ui->wet_lab->setText(weather_data.wet);
    ui->quality_lab->setText(weather_data.quality);
    ui->cityname_lab->setText(weather_data.city_name);
    ui->parent_lab->setText(weather_data.parent_name);
    ui->today_date_lab->setText(weather_data.date);
    ui->today_hlt_lab->setText(weather_data.l_temperature+"-"+weather_data.h_temperature+"℃");
    ui->today_temp_lab->setText(weather_data.temperature+"℃");
    ui->today_type_lab->setText(weather_data.weather_type);
    QPixmap pixmap(get_typepic(weather_data.weather_type));
    ui->today_pic_lab->setPixmap(pixmap);

    //接下来7天
    weather_data = datalist[1];
    QString info = weather_data.l_temperature+"-"+weather_data.h_temperature+"℃"+"   "+weather_data.weather_type;
    ui->day1_date->setText(weather_data.date);
    ui->day1_t->setText(info);
    pixmap.load(get_typepic(weather_data.weather_type));
    ui->day1_pic->setPixmap(pixmap);

    weather_data = datalist[2];
    info = weather_data.l_temperature+"-"+weather_data.h_temperature+"℃"+"   "+weather_data.weather_type;
    ui->day2_date->setText(weather_data.date);
    ui->day2_t->setText(info);
    pixmap.load(get_typepic(weather_data.weather_type));
    ui->day2_pic->setPixmap(pixmap);

    weather_data = datalist[3];
    info = weather_data.l_temperature+"-"+weather_data.h_temperature+"℃"+"   "+weather_data.weather_type;
    ui->day3_date->setText(weather_data.date);
    ui->day3_t->setText(info);
    pixmap.load(get_typepic(weather_data.weather_type));
    ui->day3_pic->setPixmap(pixmap);

    weather_data = datalist[4];
    info = weather_data.l_temperature+"-"+weather_data.h_temperature+"℃"+"   "+weather_data.weather_type;
    ui->day4_date->setText(weather_data.date);
    ui->day4_t->setText(info);
    pixmap.load(get_typepic(weather_data.weather_type));
    ui->day4_pic->setPixmap(pixmap);

    weather_data = datalist[5];
    info = weather_data.l_temperature+"-"+weather_data.h_temperature+"℃"+"   "+weather_data.weather_type;
    ui->day5_date->setText(weather_data.date);
    ui->day5_t->setText(info);
    pixmap.load(get_typepic(weather_data.weather_type));
    ui->day5_pic->setPixmap(pixmap);

    weather_data = datalist[6];
    info = weather_data.l_temperature+"-"+weather_data.h_temperature+"℃"+"   "+weather_data.weather_type;
    ui->day6_date->setText(weather_data.date);
    ui->day6_t->setText(info);
    pixmap.load(get_typepic(weather_data.weather_type));
    ui->day6_pic->setPixmap(pixmap);

    weather_data = datalist[7];
    info = weather_data.l_temperature+"-"+weather_data.h_temperature+"℃"+"   "+weather_data.weather_type;
    ui->day7_date->setText(weather_data.date);
    ui->day7_t->setText(info);
    pixmap.load(get_typepic(weather_data.weather_type));
    ui->day7_pic->setPixmap(pixmap);
}

void indexWidget::parse_weatherdata() {
    datalist.clear();
    //链接网站，获取数据
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkRequest request;

    QString citycode = get_citycode();
    QString link = "http://t.weather.itboy.net/api/weather/city/";
    QString url = link+citycode;
    request.setUrl(QUrl(url));
    QNetworkReply *reply = manager->get(request);

    connect(reply, &QNetworkReply::readyRead, [=](){
        //1.获取数据二进制数组
        QByteArray responseData = reply->readAll();
        //2.转换为对象
        QJsonParseError json_error;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData, &json_error);

        if (json_error.error != QJsonParseError::NoError) {
            qWarning() << "JSON Parse Error:" << json_error.errorString();
            reply->deleteLater();
            return;
        }

        if (!jsonDoc.isObject()) {
            qWarning() << "JSON document is not an object";
            reply->deleteLater();
            return;
        }
        QJsonObject jsonObj = jsonDoc.object();

        if (!jsonObj.contains("data")) {
            qWarning() << "'data' field not found in JSON";
            reply->deleteLater();
            return;
        }
        QJsonObject alldata_json = jsonObj["data"].toObject();

        //3.获取城市名称以及母级城市
        QJsonObject cityinfo_json = jsonObj["cityInfo"].toObject();
        QString city_name = cityinfo_json["city"].toString();
        QString parent_city_name = cityinfo_json["parent"].toString();
        qDebug() << parent_city_name;
        qDebug() << city_name;

        //4.获取日期
        QString pre_date = jsonObj["date"].toString();
        QString date = parse_date(pre_date);

        //5.获取温度
        QString temperature = alldata_json["wendu"].toString();

        //6.获取湿度
        QString wet = alldata_json["shidu"].toString();

        //7.获取空气质量
        QString quality = alldata_json["quality"].toString();

        //8.获取今日天气类型，最高最低温度，风向，级别，pm2.5
        QJsonArray forecast_arr = alldata_json["forecast"].toArray();
        QJsonObject today_json = forecast_arr[0].toObject();
        //天气类型
        QString type = today_json["type"].toString();

        //最高最低温度
        QString high_tem = today_json["high"].toString();
        QRegularExpression re("(\\d+)");
        QRegularExpressionMatchIterator it = re.globalMatch(high_tem);
        if (it.hasNext()) {
            QRegularExpressionMatch match = it.next();  // 获取第一个匹配项
            high_tem = match.captured(0);  // 提取第一个匹配的数字
        }

        QString low_tem = today_json["low"].toString();
        it = re.globalMatch(low_tem);
        if (it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            low_tem = match.captured(0);
        }
        //风向，级别
        QString wind_direction = today_json["fx"].toString();
        QString wind_grade = today_json["fl"].toString();

        //pm2.5
        int pm25_num = alldata_json["pm25"].toInt();
        QString pm25 = QString::number(pm25_num);

        //aqi
        int aqi = today_json["aqi"].toInt();

        weatherdata *today_data = new weatherdata(quality, city_name, parent_city_name, temperature, high_tem, low_tem, type, date, wind_direction, wind_grade, pm25, wet, aqi);
        datalist.append(*today_data);

        //获取6天数据
        for (int i = 1; i <= 14; i++) {
            QJsonObject nextday_json = forecast_arr[i].toObject();
            //日期
            if (i == 1) date = "明天";
            else {
                QString format = "yyyyMMdd";
                QDate nextdate_date = QDate::fromString(pre_date, format);
                nextdate_date = nextdate_date.addDays(i);
                date = nextdate_date.toString("MM/dd");
            }
            //最高最低温度
            high_tem = nextday_json["high"].toString();
            it = re.globalMatch(high_tem);
            if (it.hasNext()) {
                QRegularExpressionMatch match = it.next();  // 获取第一个匹配项
                high_tem = match.captured(0);  // 提取第一个匹配的数字
            }
            low_tem = nextday_json["low"].toString();
            it = re.globalMatch(low_tem);
            if (it.hasNext()) {
                QRegularExpressionMatch match = it.next();  // 获取第一个匹配项
                low_tem = match.captured(0);  // 提取第一个匹配的数字
            }
            //天气类型
            type = nextday_json["type"].toString();

            //aqi
            aqi = nextday_json["aqi"].toInt();
            weatherdata *next_data = new weatherdata(date, high_tem, low_tem, type, aqi);
            datalist.append(*next_data);

            // dataready();
            if(datalist.size() == 15) {draw_aqi();fill_lab();}
            // QTimer::singleShot(100, this, &indexWidget::fill_lab);
            // QTimer::singleShot(100, this, &indexWidget::draw_aqi);
            reply->deleteLater();
        }
    });

}

void indexWidget::draw_aqi() {
    // QLineSeries *aqi_series = new QLineSeries();
    aqi_series->clear();
    for (int i = 0; i < datalist.size(); i++) {
        QDateTime dateTime = QDateTime::currentDateTime().addDays(i);
        aqi_series->append(dateTime.toMSecsSinceEpoch(), datalist[i].aqi);
    }

    // qDebug() << "hhhhhh";

    // aqi_chart->setTitle("aqi空气质量曲线图");
    // chartView->setChart(aqi_chart);//向chartView中添加图标
    // chartView->resize(QSize(ui->chartwidget->size()));//重新设置chartView的大小
    // chartView->setRenderHints(QPainter::Antialiasing);//消除边缘
    // chartView->show();
}

//模糊搜索
QString indexWidget::search_cityname() {
    //从搜索框获取城市名称
    QString cityname;
    if (ui->searchbox->text().isEmpty()) {

        cityname = "房山";
    }
    else cityname = ui->searchbox->text();

    //读取txt文件看是否有该城市
    QFile cityname_file(":/cityname/cityname.txt");
    if (!cityname_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "无法打开文件:123" << cityname_file.errorString();
        return "房山";
    }
    QTextStream in(&cityname_file);


    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line == cityname) {
            cityname_file.close();
            qDebug() << "直接找到";
            return cityname;
        }
    }

    cityname_file.seek(0);
    QRegularExpression regex(QRegularExpression::escape(cityname));

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        QRegularExpressionMatch matcher = regex.match(line);
        if (matcher.hasMatch()) {
            cityname = line;
            cityname_file.close();
            qDebug() << "长度短";
            return cityname;
        }
    }

    cityname_file.seek(0);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        QRegularExpression regex(line);

        QRegularExpressionMatch matcher = regex.match(cityname);
        if (matcher.hasMatch()) {
            cityname = line;
            cityname_file.close();
            qDebug() << "长度长";
            return cityname;
        }
    }

    cityname_file.seek(0);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        for (int i = 0; i < cityname.size(); i++) {
            QRegularExpression regex(cityname[i]);
            QRegularExpressionMatch matcher = regex.match(line);
            if (matcher.hasMatch()) {
                cityname = line;
                cityname_file.close();
                qDebug() << "不匹配";
                return cityname;
            }
        }
    }

    return "房山";

}

//获取城市代码
QString indexWidget::get_citycode() {

    QString cityname = search_cityname();
    //根据城市名称获取城市代码

    //打开json文件
    QFile citycode_file(":/citycode/citycode.json");
    if (!citycode_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "无法打开文件:" << citycode_file.errorString();
        return "101010100";
    }

    //转换为二进制数组
    QByteArray citycode_bytearray = citycode_file.readAll();
    citycode_file.close();
    //转换为doc对象
    QJsonParseError jsonError;
    QJsonDocument citycode_doc = QJsonDocument::fromJson(citycode_bytearray, &jsonError);
    //转换为obj,citycode_obj为全部数据
    QJsonObject citycode_obj = citycode_doc.object();

    //查找工作
    //转换为json数组
    QJsonArray citycode_array = citycode_obj["city_code"].toArray();
    for (int i = 0; i < citycode_array.size(); i++) {
        QJsonObject province_obj = citycode_array[i].toObject();
        QJsonArray province_arr = province_obj["citys"].toArray();
        for (int j = 0; j < province_arr.size(); j++) {
            QJsonObject individual_city = province_arr[j].toObject();
            QString name = individual_city["city"].toString();
            if (name == cityname) {
                qDebug() << individual_city["code"].toString();
                return individual_city["code"].toString();
            }
        }
    }

    for (int i = 0; i < citycode_array.size(); i++) {
        QJsonObject province_obj = citycode_array[i].toObject();
        QString province_name = province_obj["Province"].toString();
        if (cityname == province_name) {
            QJsonArray province_arr = province_obj["citys"].toArray();
            QJsonObject individual_city = province_arr[0].toObject();
            return individual_city["code"].toString();
        }
    }

    return "101010100";
}

//解析日期
QString indexWidget::parse_date(QString pre_date) {
    QString year = pre_date.left(4);
    QString month = pre_date.mid(4, 2);
    QString day = pre_date.mid(6, 2);
    return year+"年"+month+"月"+day+"日";
}

QPixmap indexWidget::adjustPixmapBrightness(const QPixmap &pixmap, int delta)
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

void indexWidget::setGradient(QWidget *a, QColor start, QColor end, int duration)
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

void indexWidget::setGradient(QLabel *a, QPixmap profile, int duration)
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



