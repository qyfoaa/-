#ifndef TODAY_H
#define TODAY_H
#include<QString>
class Today{
public:
    Today(){
        date="2024-05-01";
        city="广州";
        ganmao="感冒指数";
        wendu="0℃";
        shidu="0%";
        pm25=0;
        quality="无数据";
        type="多云";
        fx="南风";
        fl="2级";
        high=30;
        low=18;
    }
    QString date;
    QString city;
    QString ganmao;
    QString wendu;
    QString shidu;
    int pm25;
    QString quality;
    QString type;
    QString fx;
    QString fl;
    int high;
    int low;

};
class Day{
public:
    Day(){
        date="2024-05-01";
        week="周三";
        type="多云";
        high=0;
        low=0;
        fx="南风";
        fl="2级";
        aqi=0;
    }
    QString date;
    QString week;
    QString type;
    QString fx;
    QString fl;
    int high;
    int low;

    int aqi;
};
#endif // TODAY_H
