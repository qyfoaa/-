#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>
#include<QJsonArray>
#include<QJsonDocument>
#include<QJsonObject>
#include<QPainter>

#define POINTY 3  //温度每升高/降低1℃的增量
#define POINT_HUI 3 //曲线描点大小
#define TEXT_HIGH
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowFlag(Qt::FramelessWindowHint);//设置窗口无边框
    setFixedSize(width(),height());

    menexit=new QMenu(this);//构建菜单
    right=new QAction();
    right->setText("退出");
    right->setIcon(QIcon(":/res/Close.png"));
    menexit->addAction(right);
    connect(right,&QAction::triggered,[=](){
        //this->close();
        qApp->exit(0);
    });
    //将控件添加到动态数组
    mWeekList<<ui->label_20<<ui->label_21<<ui->label_22<<ui->label_23<<ui->label_24<<ui->label_25;
    mDateList<<ui->label_26<<ui->label_27<<ui->label_28<<ui->label_29<<ui->label_30<<ui->label_31;
    mtqIconList<<ui->label_32<<ui->label_33<<ui->label_34<<ui->label_35<<ui->label_36<<ui->label_37;
    mtqList<<ui->label_38<<ui->label_39<<ui->label_40<<ui->label_41<<ui->label_42<<ui->label_43;
    mAqiList<<ui->label_44<<ui->label_45<<ui->label_46<<ui->label_47<<ui->label_48<<ui->label_49;
    mfxList<<ui->label_52<<ui->label_53<<ui->label_54<<ui->label_55<<ui->label_56<<ui->label_57;
    mflList<<ui->label_58<<ui->label_59<<ui->label_60<<ui->label_61<<ui->label_62<<ui->label_63;



    mTypeMap.insert("暴雪",":/res/tqi/baoxue.png");
    mTypeMap.insert("暴雨",":/res/tqi/baoyu.png");
    mTypeMap.insert("暴雨到大暴雨",":/res/tqi/baoyudaodabaoyu.png");
    mTypeMap.insert("大暴雨",":/res/tqi/dabaoyu.png");
    mTypeMap.insert("大到暴雪",":/res/tqi/dadaobaoxue.png");
    mTypeMap.insert("大到暴雨",":/res/tqi/dadaobaoyu.png");
    mTypeMap.insert("大雪",":/res/tqi/daxue.png");
    mTypeMap.insert("大雨",":/res/tqi/dayu.png");
    mTypeMap.insert("冻雨",":/res/tqi/dongyu.png");
    mTypeMap.insert("多云",":/res/tqi/duoyun.png");
    mTypeMap.insert("浮尘",":/res/tqi/fuchen.png");
    mTypeMap.insert("雷阵雨",":/res/tqi/leizhenyu.png");
    mTypeMap.insert("雷阵雨伴有冰雹",":/res/tqi/leizhenyubanyoubingbao.png");
    mTypeMap.insert("霾",":/res/tqi/mai.png");
    mTypeMap.insert("暴雨到特大大暴雨",":/res/tqi/sabaoyudaotedabaoyu.png");
    mTypeMap.insert("沙尘暴",":/res/tqi/shachenbao.png");

    mTypeMap.insert("特大暴雨",":/res/tqi/tedabaoyu.png");
    mTypeMap.insert("undefined",":/res/tqi/undefine.png");
    mTypeMap.insert("雾",":/res/tqi/wu.png");

    mTypeMap.insert("小雪到中雪",":/res/tqi/xiaoxuedaozhengxue.png");
    mTypeMap.insert("小雨",":/res/tqi/xiaoyu.png");
    mTypeMap.insert("小雨到中雨",":/res/tqi/xiaoyudaozhengyu.png");
    mTypeMap.insert("雪",":/res/tqi/xue.png");

    mTypeMap.insert("阴",":/res/tqi/yin.png");

    mTypeMap.insert("雨",":/res/tqi/yu.png");

    mTypeMap.insert("雨雪",":/res/tqi/yuxue.png");
    mTypeMap.insert("中雪",":/res/tqi/zhengxue.png");
    mTypeMap.insert("阵雨",":/res/tqi/zhengyu.png");
    mTypeMap.insert("阵雪",":/res/tqi/zhenxue.png");
    network=new QNetworkAccessManager(this);
    connect(network,&QNetworkAccessManager::finished,this,&MainWindow::onReplied);
    getWeatherInfo("101270101");
    //给标签添加事件过滤器
    //事件过滤器（EventFilter）是一种机制，它允许一个对象（称为“监视者”或“监控者”）拦截并处理另一个对象（称为“被监控者”或“目标对象”）发出的特定类型的事件
    //通过使用事件过滤器，可以在事件到达其目标对象之前拦截它，并对事件进行处理或修改。
    ui->labhigh->installEventFilter(this);//被this(当前窗口)拦截
    ui->lablow->installEventFilter(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//重写父类虚函数（父类中默认的实现是忽略右键菜单事件，重写后就可以实现右键菜单事件）
void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    //弹出右键菜单
    menexit->exec(QCursor::pos());
    event->accept();//表示事件已经处理不需要向上传递
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    mouse=event->globalPos()-this->pos();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalPos()-mouse);
}

void MainWindow::getWeatherInfo(QString city)//根据城市编码返回信息
{
    QUrl url("http://t.weather.sojson.com/api/weather/city/"+city);
    network->get(QNetworkRequest(url));//get请求完成就会是finished，上面的槽函数就会被调用
}

void MainWindow::parseJson(QByteArray &byteArry)
{
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(byteArry,&err);
    if(err.error!=QJsonParseError::NoError)
    {
        return;
    }
    QJsonObject obj=doc.object();
    qDebug()<<obj.value("message").toString();
    //解析城市和日期
    mToday.date=obj.value("date").toString();

    mToday.city=obj.value("cityInfo").toObject().value("city").toString();

    //解析yesterday
    QJsonObject objData=obj.value("data").toObject();

    QJsonObject objyesterday=objData.value("yesterday").toObject();

    mDay[0].week=objyesterday.value("week").toString();
    mDay[0].date=objyesterday.value("ymd").toString();
    mDay[0].type=objyesterday.value("type").toString();

    QString s;
    s=objyesterday.value("high").toString().split(" ").at(1);//以空格分割，取后面的18℃
    s=s.left(s.length()-1);
    mDay[0].high=s.toInt();

    QString m;
    m=objyesterday.value("low").toString().split(" ").at(1);//以空格分割，取后面的7℃
    m=m.left(m.length()-1);
    mDay[0].low=m.toInt();

    mDay[0].aqi=objyesterday.value("aqi").toDouble();
    mDay[0].fx=objyesterday.value("fx").toString();
    mDay[0].fl=objyesterday.value("fl").toString();
    //解析未来几天
    QJsonArray forecastArr=objData.value("forecast").toArray();
    for(int i=0;i<5;i++){
        QJsonObject objforecast=forecastArr[i].toObject();
        mDay[i+1].week=objforecast.value("week").toString();
        mDay[i+1].date=objforecast.value("ymd").toString();
        mDay[i+1].type=objforecast.value("type").toString();

        QString s;
        s=objforecast.value("high").toString().split(" ").at(1);//以空格分割，取后面的18℃
        s=s.left(s.length()-1);
        mDay[i+1].high=s.toInt();

        QString m;
        m=objforecast.value("low").toString().split(" ").at(1);//以空格分割，取后面的7℃
        m=m.left(m.length()-1);
        mDay[i+1].low=m.toInt();

        mDay[i+1].fl=objforecast.value("fl").toString();
        mDay[i+1].fx=objforecast.value("fx").toString();
        mDay[i+1].aqi=objforecast.value("aqi").toDouble();
    }

    //解析今天的数据
    mToday.ganmao=objData.value("ganmao").toString();
    mToday.wendu=objData.value("wendu").toString();
    mToday.shidu=objData.value("shidu").toString();
    mToday.pm25=objData.value("pm25").toDouble();
    mToday.quality=objData.value("quality").toString();
    //forecast中第一个也是今天的数据
    mToday.type=mDay[1].type;
    mToday.fx=mDay[1].fx;
    mToday.fl=mDay[1].fl;
    mToday.high=mDay[1].high;
    mToday.low=mDay[1].low;

    //更新UI
    updateUI();
}

void MainWindow::updateUI()
{
    ui->labhigh->update();
    ui->lablow->update();
    ui->label->setText(QDateTime::fromString(mToday.date,"yyyyMMdd").toString("yyyy/MM/dd")+" "+mDay[1].week);
    ui->labelcity->setText(mToday.city);

    ui->label_2->setPixmap(mTypeMap[mToday.type]);//更新图标
    //更新今天
    ui->labeltem->setText(mToday.wendu);
    ui->label_3->setText(mToday.type);
    ui->labelchange->setText(QString::number(mToday.low)+"~"+QString::number(mToday.high)+"℃");
    ui->label_4->setText("感冒指数："+ mToday.ganmao);
    ui->label_6->setText(mToday.fx);
    ui->label_8->setText(mToday.fl);
    ui->label_10->setText(QString::number(mToday.pm25));
    ui->label_16->setText(mToday.shidu);
    ui->label_19->setText(mToday.quality);
    //更新六天
    for(int i=0;i<6;i++){
        mWeekList[i]->setText("周"+ mDay[i].week.right(1));
        ui->label_20->setText("昨天");
        ui->label_21->setText("今天");
        ui->label_22->setText("明天");
        QStringList str=mDay[i].date.split("-");//将-左右的字符分割出来存在str数值中
        mDateList[i]->setText(str[1]+"/"+str[2]);
        mtqList[i]->setText(mDay[i].type);


        mtqIconList[i]->setPixmap(mTypeMap[mDay[i].type]);
        mtqIconList[i]->setFixedSize(64, 34);
        mtqIconList[i]->setScaledContents(true);


        if(mDay[i].aqi>=0&&mDay[i].aqi<=50){
            mAqiList[i]->setText("优");
            mAqiList[i]->setStyleSheet("background-color:rgb(121,184,0);");
        }
        else if(mDay[i].aqi>50&&mDay[i].aqi<=100){
            mAqiList[i]->setText("良");
            mAqiList[i]->setStyleSheet("background-color:rgb(255,187,23);");
        }
        else if(mDay[i].aqi>100&&mDay[i].aqi<=150){
            mAqiList[i]->setText("轻度");
            mAqiList[i]->setStyleSheet("background-color:rgb(255,87,97);");
        }
        else if(mDay[i].aqi>150&&mDay[i].aqi<=200){
            mAqiList[i]->setText("中度");
            mAqiList[i]->setStyleSheet("background-color:rgb(235,27,17);");
        }
        else if(mDay[i].aqi>200&&mDay[i].aqi<=250){
            mAqiList[i]->setText("重度");
            mAqiList[i]->setStyleSheet("background-color:rgb(170,0,0);");
        }
        else {
            mAqiList[i]->setText("严重");
            mAqiList[i]->setStyleSheet("background-color:rgb(110,0,0);");
        }
        mfxList[i]->setText(mDay[i].fx);
        mflList[i]->setText(mDay[i].fl);
    }



}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    //监视的是哪一个控件，以及发生的事件
    if(watched==ui->labhigh && event->type()== QEvent::Paint){
        painHigh();
    }
    if(watched==ui->lablow && event->type()== QEvent::Paint){
        PainLow();
    }
    return QWidget::eventFilter(watched,event);//其他控件做默认处理
}

void MainWindow::painHigh()
{
    QPainter painter(ui->labhigh);
    painter.setRenderHint(QPainter::Antialiasing,true);//抗锯齿
    //获取x坐标
    int PointX[6]={0};
    for(int i=0; i<6; i++){
        PointX[i]=mWeekList[i]->pos().x()+mWeekList[i]->width()/2;

    }
    //获取y坐标
    int wenduSum=0;
    int wenduavg=0;
    for(int i=0;i<6;i++){
        wenduSum+=mDay[i].high;
    }
    wenduavg=wenduSum/6;
    int pointY[6]={0};
    int yCenter=ui->labhigh->height()/2;
    for(int i=0;i<6;i++){
        pointY[i]=yCenter-((mDay[i].high-wenduavg)*POINTY);
    }

    //开始绘制
    QPen pen=painter.pen();
    pen.setWidth(1);
    pen.setColor(QColor(255,170,0));
    painter.setPen(pen);//画家拿起了画笔
    painter.setBrush(QColor(255,170,0));
    for(int i=0;i<6;i++){
        painter.drawEllipse(QPoint(PointX[i],pointY[i]),POINT_HUI,POINT_HUI);
        painter.drawText(PointX[i],pointY[i]-5,QString::number(mDay[i].high)+"°");
    }
    for(int i=0;i<5;i++){
        if(i==0){
            pen.setStyle(Qt::DotLine);
            painter.setPen(pen);
        }
        else{
            pen.setStyle(Qt::SolidLine);
            painter.setPen(pen);
        }
        painter.drawLine(QPoint(PointX[i],pointY[i]),QPoint(PointX[i+1],pointY[i+1]));
    }
}

void MainWindow::PainLow()
{
    QPainter painter(ui->lablow);
    painter.setRenderHint(QPainter::Antialiasing,true);//抗锯齿
    //获取x坐标
    int PointX[6]={0};
    for(int i=0; i<6; i++){
        PointX[i]=mWeekList[i]->pos().x()+mWeekList[i]->width()/2;

    }
    //获取y坐标
    int wenduSum=0;
    int wenduavg=0;
    for(int i=0;i<6;i++){
        wenduSum+=mDay[i].low;
    }
    wenduavg=wenduSum/6;
    int pointY[6]={0};
    int yCenter=ui->lablow->height()/2;
    for(int i=0;i<6;i++){
        pointY[i]=yCenter-((mDay[i].low-wenduavg)*POINTY);
    }

    //开始绘制
    QPen pen=painter.pen();
    pen.setWidth(1);
    pen.setColor(QColor(0,255,255));
    painter.setPen(pen);//画家拿起了画笔
    painter.setBrush(QColor(0,255,255));
    for(int i=0;i<6;i++){
        painter.drawEllipse(QPoint(PointX[i],pointY[i]),POINT_HUI,POINT_HUI);
        painter.drawText(PointX[i],pointY[i]-5,QString::number(mDay[i].low)+"°");
    }
    for(int i=0;i<5;i++){
        if(i==0){
            pen.setStyle(Qt::DotLine);
            painter.setPen(pen);
        }
        else{
            pen.setStyle(Qt::SolidLine);
            painter.setPen(pen);
        }
        painter.drawLine(QPoint(PointX[i],pointY[i]),QPoint(PointX[i+1],pointY[i+1]));
    }
}

void MainWindow::onReplied(QNetworkReply *reply)
{
    qDebug()<<"onReplied successful";
    //响应的信息，响应的状态码为200代表请求成功
    int status_code=reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    qDebug()<<"operation:"<<reply->operation();  //请求方式
    qDebug()<<"status_code:"<<status_code;       //状态码
    qDebug()<<"url:"<<reply->url();              //url
    qDebug()<<"raw header:"<<reply->rawHeaderList();  //响应header
    if(reply->error()!=QNetworkReply::NoError||status_code!=200)
    {
        qDebug()<<reply->errorString().toLatin1().data();
        //errorString()返回一个QString类型字符串，toLatin1()将该QString转换为使用Latin-1编码QByteArray类型
        //data()函数返回QByteArray的数据指针
        QMessageBox::warning(this,"提示","请求失败",QMessageBox::Ok);
    }
    else{
        QByteArray reply_data=reply->readAll();
        QByteArray byteArray=QString(reply_data).toUtf8();
        qDebug()<<"read All:"<<byteArray.data();
        parseJson(byteArray);
    }
    reply->deleteLater();//把堆上的内存释放防止内存泄漏
}








