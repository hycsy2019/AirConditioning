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
    MainWindow(QWidget *parent = Q_NULLPTR);/*�1�7�1�7�1�7�1�7�1�7�1�7�1�7�ք1�7�4�4�1�7�1�7�1�7�1�7�1�7�1�7�1�7�1�9�1�7�1�7�1�7�1�7�1�7�1�7�1�7�1�7�1�7�1�7�1�7�0�3�1�7�1�7*/
      QMessageBox *OnServerTip;
    void change_indoor_temp(float new_temp);/*�1�7�0�3�1�7�1�7�1�7�1�7�1�7�1�7�0�9�1�7*/
    void change_wifi(bool new_wifi);/*�1�7�1�7�1�7�1�7�1�7�1�7�1�7�1�7�0�8�0�0*/
    void change_mode(bool new_mode);/*�1�7�݄1�7�1�7�1�7�1�7�1�7�0�0�0�4*/
    void change_limit_temp(int max_temp, int min_temp);/*�1�7�0�3�1�7�1�7�0�9�1�7�1�7�1�7�1�7�1�7�1�7�1�7*/
    void power_saving(bool no_wind);/*�1�7�1�7�1�7�1�7�1�7�0�9�1�7�1�7�1�7�1�7�1�7�1�7�1�9�1�7�0�0*/
    void update_total_money(float money);/*�����ܷ���*/
    void change_tar_temp(int dt);/*�1�7�0�3�1�7�0�7�1�7�1�7�1�7�0�9�1�7*/
    void change_wind(int level);/*�1�7�0�3�1�7�1�7�1�7�1�7�1�7*/
    void all_disabled();/*�1�7�0�3�1�7�1�7�1�7�1�7�1�7�0�8�0�0�1�7�1�7�1�7�1�7�0�2�1�7�1�7��*/
    void all_enabled();/*�1�7�1�7�1�7�1�7�0�8�0�0*/
    void update_unifee(float unifee);
    void update_current_money(float cur_fee);

    LoginForm *login;

public slots:
    void switch_to_w1() { client->ChangeFanAndWindSpeed(1,1); };
    void switch_to_w2() { client->ChangeFanAndWindSpeed(1,2); };
    void switch_to_w3() { client->ChangeFanAndWindSpeed(1,3); };
    void sub_temp() { change_tar_temp(-1); };
    void add_temp() { change_tar_temp(1); };
    void set_time();//�1�7�1�7�1�7�0�1�1�7�0�2�0�2�1�7�1�7
    void switch_power(){
        if(client->state){
             power == 0 ? client->createOnJson() : client->createOffJson();
             power_ui();
        }
    };//��Դ��ť�ۺ���by csy 4.11
    void reconnect_slot(){
        if(client->state){
            QMessageBox::warning(NULL,"��ܰ��ʾ","Ŀǰ���Ʒ�������������,��������");
        }
        else
            client->CreateWebscoketlink();};//�����ۺ���by csy 4.11

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    Ui::MainWindow ui;

    void power_ui();
    bool power = 0;//�1�7�1�7�1�7�1�9�1�7�0�8�0�0
    double ui_tar_temp;//�0�7�1�7�1�7�1�7�0�9�1�7�1�7�1�7�0�5
    double ui_indoor_temp;//�1�7�1�7�1�7�1�7�1�7�0�9�1�7�1�7�1�7�0�5
    int ui_expense;//�1�7�1�2�0�7�1�7�1�7�1�7�1�7�1�7�0�5
    int ui_min_temp;
    int ui_max_temp;
    QTimer *timer;
    QPoint mousePoint;
    bool mouse_press;
};
