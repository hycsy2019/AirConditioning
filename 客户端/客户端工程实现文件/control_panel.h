#pragma once

#include <QtWidgets/QMainWindow>
#include<qpropertyanimation.h>
#include <QJsonValue>
#include <QMessageBox>
#include <QDate>
#include<QTime>
#include<QTimer>
#include <QMouseEvent>
#include "QJsonArray"
#include "QJsonDocument"
#include "QByteArray"
#include "ui_control_panel.h"
#include "client.h"
#include "IpCheck.h"

extern Client *client;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = Q_NULLPTR);/*�������幹�캯�������ڽ�����������ʼ��*/
      QMessageBox *OnServerTip;
    void change_indoor_temp(float new_temp);/*�ı������¶�*/
    void change_wifi(bool new_wifi);/*��������״̬*/
    void change_mode(bool new_mode);/*�л�����ģʽ*/
    void change_limit_temp(int max_temp, int min_temp);/*�ı��¶�������*/
    void power_saving(bool no_wind);/*�����Ƿ������޷�̬*/
    void update_total_money(float money);/*更新总费用*/
    void change_tar_temp(int dt);/*�ı�Ŀ���¶�*/
    void change_wind(int level);/*�ı�����*/
    void all_disabled();/*�Ͽ�����״̬����ť��Ч*/
    void all_enabled();/*����״̬*/
    void update_unifee(float unifee);
    void update_current_money(float cur_fee);

    LoginForm *login;

public slots:
    void switch_to_w1() { client->ChangeFanAndWindSpeed(1,1); };
    void switch_to_w2() { client->ChangeFanAndWindSpeed(1,2); };
    void switch_to_w3() { client->ChangeFanAndWindSpeed(1,3); };
    void sub_temp() { change_tar_temp(-1); };
    void add_temp() { change_tar_temp(1); };
    void set_time();//���õ�ǰʱ��
    void switch_power(){
        if(client->state){
             power == 0 ? client->createOnJson() : client->createOffJson();
             power_ui();
        }
    };//电源按钮槽函数by csy 4.11
    void reconnect_slot(){
        if(client->state){
            QMessageBox::warning(NULL,"温馨提示","目前与云服务器连接正常,无需重连");
        }
        else
            client->CreateWebscoketlink();};//重连槽函数by csy 4.11

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    Ui::MainWindow ui;

    void power_ui();
    bool power = 0;//���ػ�״̬
    double ui_tar_temp;//Ŀ���¶���ʾ
    double ui_indoor_temp;//�����¶���ʾ
    int ui_expense;//�ۼƻ�����ʾ
    int ui_min_temp;
    int ui_max_temp;
    QTimer *timer;
    QPoint mousePoint;
    bool mouse_press;
};
