#pragma once
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore/QObject>
#include <QWebSocket>
#include <QTimer>
#include <QString>
#include <QPushButton>
#include <QGridLayout>
#include <QMessageBox>
#include <sstream>
#include <iomanip>
#include "IpCheck.h"

const double EPS = 0.0000001;

class Client : public QMainWindow
{
	Q_OBJECT

public:
	Client();
	~Client();

private:
	friend class MainWindow;

	//通信
	QWebSocket* _pWebsocket;     //socket接口
	bool connectStatus;      //连接状态
	QTimer *m_timer;     //通信连接计时器
	QString ServerUrl;  //服务器地址
	QString SendMsg;   //送给服务器的消息
	int totalcount;   //允许最大重复连接次数

    int state;    //从控机网络连接状态
    int powersave;

	//房间
    int room_id=102;    //房间ID
    int windlevel=2;   //当前风速    "level" : 0// 1 2 3
    //室温变化控制次数的温度间隔量
    float temp_change;
    int depart = 0;   //调度离开

    int last_wind_level_power_saving = 1;   //节电时上一次风速

    float tar_temp=25;  //预期温度（服务器：当前温度）
    float last_tar_temp = tar_temp;
    int last_wind_level;

    float cur_temp=27;  //目前温度
    float initial_cur_temp;   //判断的最初的室温
    float last_cur_temp = cur_temp;   //存储上一次的相差一度的温度数据

    int tar_cnt = 1;

    long long int time_stamp;  //时间间隔

    int startflag = 1;

    float fee=0;      //当前费用(累计)
    float unit_fee = 0;   //单价
    float cur_fee = 0;    //本次费用
	int mode=0;   //当前中央空调温度模式,0:冷  1：热
    int status = 0;   //从控机开关机状态
    int oncount=1;   //开机前即使温度未变，发送一次报文
    QTimer *TempTimer;   //实时更新并发送房间温度

    int CalcTemp();    //房间温度检测计算    0：随机值   1：模式变化值
	QString createOnJson();    //创建开机json格式
	QString createOffJson();    //创建关机json格式

    QMessageBox *OnServerTip;

    QLabel *tipLbl;  //"欢迎登录"标签
    LoginForm *login;

    int runflag = 1;

    QString ip;
    QString port;

public slots:
    void CreateWebscoketlink();   //建立连接
	void OnConnected();
	void OntextRec(QString msg);
	void OnDisconnected();
	void reconnect();
    void ChangeFanAndWindSpeed(int kind = 0,int level = 1);   //0:温度  1:风速  设置风速与温度
    void ResendTemp();   // 温度自动发送  kind:0  未连接网络  kind:1已连接网络
    void ResendWind(int level);
};
#endif // MAINWINDOW_H
