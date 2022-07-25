#include "control_panel.h"
#include "IpCheck.h"
#include "QDebug"

//临时参数值
extern QString str_ip;              //ip
extern QString str_port;             //端口
extern QString str_roomid;           //房间号
extern QString str_curtemp;          //房间温度
extern QString str_tartemp;           //目标温度
extern QString str_windlevel;         //风速

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    login = new LoginForm();
    if(login->exec() != QDialog::Accepted)
        exit(0);
    //界面参数赋值
    client = new Client();

    //参数初始化
    client->room_id = str_roomid.toInt();
    client->cur_temp = str_curtemp.toFloat();
    client->last_cur_temp = client->cur_temp;   //临时温度设定
    client->initial_cur_temp = client->cur_temp;    //判断的室温的条件
    client->tar_temp = str_tartemp.toInt();
    client->windlevel = str_windlevel.toInt();
    client->last_wind_level_power_saving = client->windlevel;
    client->last_wind_level = client->windlevel;
    client->ip = str_ip;
    client->port = str_port;

    client->ServerUrl = "ws://";
    client->ServerUrl += str_ip;
    client->ServerUrl += (":" + str_port);
    client->ServerUrl += ("/" + str_roomid);

    ui_tar_temp = client->tar_temp;
    ui_indoor_temp = client->cur_temp;
    ui_expense = client->fee;
    ui_min_temp = 15;
    ui_max_temp = 35;
    this->setWindowFlags(Qt::FramelessWindowHint);

    ui.setupUi(this);

    update_total_money(client->fee);
    update_current_money(client->cur_fee);
    change_indoor_temp(client->cur_temp);

    ui.tar_temp->setText(QString::number(client->tar_temp));
    this->update();

    change_mode(client->mode);
    change_limit_temp(ui_max_temp, ui_min_temp);

    /*设置房间号*/
    ui.title->setText(("空调控制面板-")+ QString::number(client->room_id));

    //重连按钮by csy 4.11
    ui.reconnect->setFlat(true);
    ui.reconnect->hide();

    /*每隔一分钟设置一次时间*/
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(set_time()));
    set_time();

    client->CreateWebscoketlink();
}

//设置当前时间
void MainWindow::set_time()
{
	QTime current_time = QTime::currentTime();
	if (current_time.hour() >= 12)
	{
		ui.time->setText(QString::number(current_time.hour() - 12) + ':' + (current_time.minute()<10?"0":"")+QString::number(current_time.minute()));
		ui.pm_am->setText("PM");
	}
	else
	{
		ui.time->setText(QString::number(current_time.hour()) + ':' + (current_time.minute() < 10 ? "0" : "")+QString::number(current_time.minute()));
		ui.pm_am->setText("AM");
	}
	timer->start(60000);
}

//改变室内温度
//new_temp:变化后的室内温度
void MainWindow::change_indoor_temp(float new_temp)
{
    ui.indoor_temp->setText(QString::number(new_temp,'f',1));
    qDebug() << "newtemp = " << QString::number(new_temp,'f',1) << endl;
	this->update();
}

//改变目标温度
void MainWindow::change_tar_temp(int dt)
{
    if (ui_tar_temp + dt > ui_min_temp && ui_tar_temp + dt < ui_max_temp)
	{
		ui_tar_temp += dt;

		ui.tar_temp->setText(QString::number(ui_tar_temp));
		this->update();

        client->last_tar_temp = client->tar_temp;
        client->tar_temp += dt;
        client->ChangeFanAndWindSpeed(0);


        //客户机的显示屏在发出报文后不需要等到回复就可以变化，但是客户机的温度模拟器需要等待服务器的回复
        change_tar_temp(client->tar_temp);
        qDebug() << "::: tar_temp = " <<client->tar_temp << endl;
	}
}

//更改连接状态
//new_wifi：更改后的连接状态，1为正常连接，0为与服务器断开连接
void MainWindow::change_wifi(bool new_wifi)
{
	if (new_wifi)
		ui.wifi->setPixmap(QPixmap(QString::fromUtf8(":/source/wifi_on.png")));
	else
		ui.wifi->setPixmap(QPixmap(QString::fromUtf8(":/source/wifi_off.png")));
	this->update();
}

//切换工作模式
//new_mode：切换到的工作状态，0为制冷，1为制热
void MainWindow::change_mode(bool new_mode)
{
	if (new_mode)
		ui.mode->setPixmap(QPixmap(QString::fromUtf8(":/source/sun.png")));
	else
        ui.mode->setPixmap(QPixmap(QString::fromUtf8(":/source/snow.png")));
	this->update();
}

//改变温度上下限
//max_temp:最高温度
//min_temp:最低温度
void MainWindow::change_limit_temp(int max_temp, int min_temp)
{
	ui.max_temp->setText(QString::number(max_temp));
    ui.min_temp->setText(QString::number(min_temp));
	this->update();
}

//开关机
void MainWindow::power_ui()
{
        if (power)//关机
        {
                QPropertyAnimation *powerOff = new QPropertyAnimation(ui.title, "geometry");
                powerOff->setDuration(1000);
                powerOff->setStartValue(QRect(0, 0, 801, 71));
                powerOff->setEndValue(QRect(0, 0, 801, 501));
                powerOff->start();
                power = 0;
                ui.reconnect->show();
        }
        else//开机
        {
            if(client->state == 1){
                power = 1;
                all_enabled();
                QPropertyAnimation *powerOn = new QPropertyAnimation(ui.title, "geometry");
                powerOn->setDuration(1000);
                powerOn->setStartValue(QRect(0, 0, 801, 501));
                powerOn->setEndValue(QRect(0, 0, 801, 71));
                powerOn->start();
            }
            else
                ui.reconnect->show();
        }
}

//变风速
void MainWindow::change_wind(int level)
{
    switch (level)
    {
    case 0:
            power_saving(1);
            break;
    case 1:
            ui.wind1->setStyleSheet(QStringLiteral("border-image: url(:/source/wind1_on.png);"));
            ui.wind2->setStyleSheet(QStringLiteral("border-image: url(:/source/wind2_off.png);"));
            ui.wind3->setStyleSheet(QStringLiteral("border-image: url(:/source/wind3_off.png);"));
            //重置需调用的室温变化函数
            client->temp_change = 1.0/1800;
            break;
    case 2:
            ui.wind1->setStyleSheet(QStringLiteral("border-image: url(:/source/wind1_off.png);"));
            ui.wind2->setStyleSheet(QStringLiteral("border-image: url(:/source/wind2_on.png);"));
            ui.wind3->setStyleSheet(QStringLiteral("border-image: url(:/source/wind3_off.png);"));
            //重置需调用的室温变化函数
            client->temp_change = 1.0/1200;
            break;
    case 3:
            ui.wind1->setStyleSheet(QStringLiteral("border-image: url(:/source/wind1_off.png);"));
            ui.wind2->setStyleSheet(QStringLiteral("border-image: url(:/source/wind2_off.png);"));
            ui.wind3->setStyleSheet(QStringLiteral("border-image: url(:/source/wind3_on.png);"));
            //重置需调用的室温变化函数
            client->temp_change = 1.0/600;
            break;
    }
    //取消节电标识
    if(level != 0){
        power_saving(0);
    }
    this->update();
}


//设置是否处于无风态
//no_wind:为1处于无风态，否则为0
void MainWindow::power_saving(bool no_wind)
{
        if (no_wind)
        {
            ui.no_wind->setPixmap(QPixmap(QString::fromUtf8(":/source/power_saving.png")));
            ui.money_text_off->show();
            ui.money_text_on->hide();
            ui.disconnected->hide();
        }
        else
        {
            ui.no_wind->setPixmap(QPixmap(QString::fromUtf8(":/source/no_saving.png")));
            ui.money_text_on->show();
            ui.money_text_off->hide();
            ui.disconnected->hide();
        }
        this->update();
}

//更新总计费
//total_money:消费额乘100
void MainWindow::update_total_money(float money)
{
    ui.total_money->setText(QString::number(money,'f',2));
    qDebug() << "total fee = " << QString::number(money,'f',2) << endl;
	this->update();
}

//更新此次计费
//current_money:消费额乘100
void MainWindow::update_current_money(float money)
{
    ui.current_money->setText(QString::number(money,'f',2));
    qDebug() << "current fee = " << QString::number(money,'f',2) << endl;
    this->update();
}

//更新单价
void MainWindow::update_unifee(float unifee)
{
    ui.uni_fee->setText(QString::number(unifee,'f',2));
    qDebug() << "test uni_fee = " << QString::number(unifee,'f',2) << endl;
    this->update();
}

//断开连接状态，按钮无效
void MainWindow::all_disabled()
{
    if(power)//开机态，所有按钮变灰
    {
        ui.add->hide();
        ui.sub->hide();

        change_wifi(0);
        ui.wind1->setStyleSheet(QStringLiteral("border-image: url(:/source/wind1_off.png);"));
        ui.wind2->setStyleSheet(QStringLiteral("border-image: url(:/source/wind2_off.png);"));
        ui.wind3->setStyleSheet(QStringLiteral("border-image: url(:/source/wind3_off.png);"));

        ui.wind1->setEnabled(false);
        ui.wind2->setEnabled(false);
        ui.wind3->setEnabled(false);

        ui.mode->hide();

        power_saving(0);

        ui.money_text_on->hide();
        ui.money_text_off->hide();
        ui.disconnected->show();
        ui.power->setEnabled(false);
        ui.reconnect->show();
    }
    //出现重连按钮 ，电源键失效by csy 4.11
    ui.reconnect->show();
    ui.power->setEnabled(false);
}

//已连接状态，启动按钮
void MainWindow::all_enabled()
{
    if(power)
    {
        ui.add->show();
        ui.sub->show();

        change_wind(client->windlevel);
        change_wifi(1);
        ui.wind1->setEnabled(true);
        ui.wind2->setEnabled(true);
        ui.wind3->setEnabled(true);

        QString s;
        if (client->mode==1)
            s=":/source/sun.png";
        else
            s=":/source/snow.png";
        ui.mode->setPixmap(QPixmap(s));
        ui.mode->show();
    }
    //隐藏重连按钮，启动电源键by csy 4.11
    ui.reconnect->hide();
    ui.power->setEnabled(true);
}

//以下函数实现鼠标拖动窗口
void MainWindow::mousePressEvent(QMouseEvent *event)
{

	if ((event->button() == Qt::LeftButton)) {
		mouse_press = true;
		mousePoint = event->globalPos() - this->pos();
		//        event->accept();
	}
	else if (event->button() == Qt::RightButton) {
		//如果是右键
		this->close();
	}
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
	if (mouse_press) {
		move(event->globalPos() - mousePoint);
	}
}
void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
	mouse_press = false;
}


