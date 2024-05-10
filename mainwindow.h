#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "today.h"
#include <QMainWindow>
#include <QContextMenuEvent>
#include <QMenu>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QLabel>
#include <QList>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
protected:
    void contextMenuEvent(QContextMenuEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    void getWeatherInfo(QString city);

    void parseJson(QByteArray &byteArry);//解析数据

    void updateUI();
    //重写父类的eventfieter
    bool eventFilter(QObject *watched,QEvent *event);
    //绘制高低温曲线的函数
    void painHigh();
    void PainLow();

private:
    Ui::MainWindow *ui;
    QMenu *menexit;//右键菜单
    QAction *right;//退出的行为
    QPoint mouse;//鼠标雨窗口左上角的偏移
    QNetworkAccessManager *network;
    void onReplied(QNetworkReply *reply);//槽函数

    Today mToday;
    Day mDay[6];

    //星期和日期
    QList<QLabel*> mWeekList;
    QList<QLabel*> mDateList;

    //天气和天气图标
    QList<QLabel*> mtqList;
    QList<QLabel*> mtqIconList;

    //空气污染指数
    QList<QLabel*> mAqiList;

    //风力和风向
    QList<QLabel*> mflList;
    QList<QLabel*> mfxList;

    QMap<QString,QString> mTypeMap;

    //

};
#endif // MAINWINDOW_H
