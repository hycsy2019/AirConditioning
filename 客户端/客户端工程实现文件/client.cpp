#include <QJsonValue>
#include <QMessageBox>
#include <QDate>
#include<QDebug>
#include "control_panel.h"
#include "client.h"
#include "QJsonObject"
#include "QJsonArray"
#include "QJsonDocument"
#include "QByteArray"
#include "QInputDialog"
#include "IpCheck.h"
#include "cmath"

extern MainWindow *w;

Client::Client()
{
    /*连接*/
    OnServerTip = new QMessageBox(this);
    OnServerTip->setObjectName("OnServerTip");
    OnServerTip->setFixedHeight(80);
    OnServerTip->setFixedWidth(160);
    OnServerTip->setText(tr("正在与云服务器进行连接..."));
    OnServerTip->addButton(QMessageBox::Ok);
    OnServerTip->button(QMessageBox::Ok)->hide();
    OnServerTip->hide();

	m_timer = new QTimer();
    TempTimer = new QTimer();
    connect(TempTimer, SIGNAL(timeout()), this, SLOT(ResendTemp()), Qt::AutoConnection);
    _pWebsocket = 0;
    powersave = 0;
    state = 0;   //初始状态:脱机状态（未连接）
	totalcount = 5; //设置重复
}

/*
 * CalcTemp:基于该函数每五秒钟被调用一次，并且到达目标温度时，进行自动的回温或者降温
 * 那么windlevel = 1时，次数为36次，即温度变化为
 * windlevel = 2时，次数为24次，温变为0.042
 * windlevel = 3时，次数为12次，温变为0.084
 * 并且如果设置的风速发生改变时，进行重新置数temp_change，开始新的温变间隔
*/
int Client::CalcTemp()
{
    //已经开机时
    if(status == 1){
        //正在节电时的室温变化
        if(powersave){
            //制热时，进行降温
            if(mode == 1){
                cur_temp -= 1.0/1200;
            }
            //制冷时，进行升温
            else{
                cur_temp += 1.0/1200;
            }
        }
        else{
            float difftemp = temp_change;
            //制冷
            if (mode == 0) {
                //先前达到目标温度时
                if(cur_temp == tar_temp){
                    if(cur_temp > initial_cur_temp);
                    else
                        cur_temp += difftemp;
                    qDebug() << "mode 77"<< endl;
                }
                else if(depart == 1 && windlevel != 0){
                    if (cur_temp > tar_temp  && cur_temp - difftemp < tar_temp){
                        last_cur_temp = cur_temp;
                        cur_temp = tar_temp;
                    }
                    else if(cur_temp > tar_temp)
                        cur_temp -= difftemp;
                }
                else if(cur_temp < tar_temp && windlevel == 0){
                    cur_temp += 1.0/1200;
                }
            }
            //制热
            else {
                //先前达到目标温度时
                if(cur_temp == tar_temp){
                    if(cur_temp < initial_cur_temp);
                    else
                        cur_temp -= difftemp;
                }
                else if(depart == 1 && windlevel != 0){
                    if (cur_temp < tar_temp  && cur_temp + difftemp > tar_temp){
                        last_cur_temp = cur_temp;
                        cur_temp = tar_temp;
                    }
                    else if(cur_temp < tar_temp)
                        cur_temp += difftemp;
                }
                else if(cur_temp > tar_temp && windlevel == 0){
                    cur_temp -= 1.0/1200;
                }
            }
        }
        qDebug() << "Cur_temp = " << cur_temp << endl;

        if(abs(cur_temp - tar_temp) < EPS && !powersave)
            oncount = 1;
        //空调不工作时判断温度变化是否超过0.1度
        if(abs((last_cur_temp) - (cur_temp)) >= 0.1){
            last_cur_temp = cur_temp;   //更新已到达的相差0.1度的温度
            return 2;
        }
        else
            return 1;
    }
    else{
            //关机后的温度变化
            if(startflag == -1){
                //制热时，进行降温
                if(mode == 1){
                    cur_temp -= 1.0/1200;
                }
                //制冷时，进行升温
                else{
                    cur_temp += 1.0/1200;
                }
                if(abs((last_cur_temp) - (cur_temp)) >= 0.1){
                    last_cur_temp = cur_temp;   //更新已到达的相差0.1度的温度
                    return 2;
                }
                else
                    return 1;
            }
    }
}

QString Client::createOnJson()   //构造开机报文
{
    ResendTemp();
    //CalcTemp();
	QJsonObject json;
    json.insert("room_id", room_id);
	json.insert("event", "on");

    QString timestamp = QString::number(QDateTime::currentMSecsSinceEpoch());
    json.insert("timestamp", timestamp);

	QJsonObject subjson;
    subjson.insert("cur_tem", (cur_temp));
    subjson.insert("tar_tem", (tar_temp));
	subjson.insert("wind", windlevel);

	//构建Json数组
	json.insert("data", QJsonValue(subjson));

	QJsonDocument document;
	document.setObject(json);
	QByteArray secArray = document.toJson(QJsonDocument::Compact);
    qDebug() << "构造的开机报文是" << QString(secArray) << endl;
	QString str = secArray;

	//发送开机信息
	_pWebsocket->sendTextMessage(str);
	return str;
}

QString Client::createOffJson()   //构造关机报文
{
    QJsonObject json;
    json.insert("room_id", room_id);
    json.insert("event", "off");

    QString timestamp = QString::number(QDateTime::currentMSecsSinceEpoch());
    json.insert("timestamp", timestamp);

    QJsonObject subjson;
    subjson.insert("cur_tem", (cur_temp));
    subjson.insert("tar_tem", (tar_temp));

    //构建Json数组
    json.insert("data", QJsonValue(subjson));

    QJsonDocument document;
    document.setObject(json);
    QByteArray secArray = document.toJson(QJsonDocument::Compact);
    qDebug() << "构造的关机报文是" << QString(secArray) << endl;
    QString str = secArray;

    startflag = -1;

    //发送关机信息
    _pWebsocket->sendTextMessage(str);
    return str;
}

void Client::ChangeFanAndWindSpeed(int kind,int level)
{
    QJsonObject json;
    json.insert("room_id", room_id);
    json.insert("event", "set");
    QString timestamp = QString::number(QDateTime::currentMSecsSinceEpoch());
    json.insert("timestamp", timestamp);

    QJsonObject subjson;

    subjson.insert("cur_tem", (cur_temp));
    subjson.insert("tar_tem", (tar_temp));

    if(kind){
        subjson.insert("wind", level);
        last_wind_level = windlevel;
        last_wind_level_power_saving = windlevel;
        windlevel = level;
        w->change_wind(windlevel);
        powersave = 0;
    }
    subjson.insert("wind", windlevel);
    //构建Json数组
    json.insert("data", QJsonValue(subjson));

    QJsonDocument document;
    document.setObject(json);
    QByteArray secArray = document.toJson(QJsonDocument::Compact);
    qDebug() << "构造的温度与风速变化报文是" << QString(secArray) << endl;

    QString str = secArray;
    _pWebsocket->sendTextMessage(str);
}

void Client::ResendWind(int level)
{
    QJsonObject json;
    json.insert("room_id", room_id);
    json.insert("event", "set");
    QString timestamp = QString::number(QDateTime::currentMSecsSinceEpoch());
    json.insert("timestamp", timestamp);
    QJsonObject subjson;
    subjson.insert("cur_tem", cur_temp);
    subjson.insert("tar_tem", tar_temp);
    subjson.insert("wind", level);
    //构建Json数组
    json.insert("data", QJsonValue(subjson));
    QJsonDocument document;
    document.setObject(json);
    QByteArray secArray = document.toJson(QJsonDocument::Compact);
    qDebug() << "进入或者退出省电模式的报文：" << QString(secArray) << endl;
    QString str = secArray;
    _pWebsocket->sendTextMessage(str);
    return ;
}

void Client::ResendTemp()
{
    //在发送之前进行房间温度的模拟检测
    int IsSendTemp = CalcTemp();
    w->change_indoor_temp(cur_temp);
    //当机器启动的时候进行的节电的判断
    if(status == 1){
        //判断是否进行节电模式
        if((mode == 1 && tar_temp <= cur_temp )|| (mode == 0 && tar_temp >= cur_temp)){
            if(windlevel != 0)
                last_wind_level_power_saving = windlevel;
            powersave = 1;
            windlevel = 0;   //风速自动为0
            if(runflag){
                ResendWind(0);
                runflag = 0;
            }
            w->change_wind(windlevel);   //增加节点标识
            return ;
        }
        if(powersave == 1 && (mode == 0 ? (abs((cur_temp) - (tar_temp)) >= 1):(abs((cur_temp) - (tar_temp)) >= 1))){
                powersave = 0;
                windlevel = last_wind_level_power_saving;
                ResendWind(windlevel);
                w->change_wind(windlevel);   //取消节电标识
               // qDebug() << "change_wind: windlevel = " << windlevel << endl;
        }
    }
    //未超过0.1度
    if(IsSendTemp == 1 && oncount != 1){
        return;
    }
    oncount = 0;
    //超过一度{当手动触动或者变化超过一度时进行报文发送}
    QJsonObject json;
    json.insert("room_id", room_id);
    json.insert("event", "tem");
    QString timestamp = QString::number(QDateTime::currentMSecsSinceEpoch());
    json.insert("timestamp", timestamp);

    QJsonObject subjson;
    subjson.insert("cur_tem", cur_temp);
    //构建Json数组
    json.insert("data", QJsonValue(subjson));

    QJsonDocument document;
    document.setObject(json);
    QByteArray secArray = document.toJson(QJsonDocument::Compact);
    qDebug() << "构造的温度变化报文是" << QString(secArray) << endl;

    QString str = secArray;
    if(startflag == 1 || status == 1 || startflag == -1)
        _pWebsocket->sendTextMessage(str);
    if(startflag != -1)
        startflag = 0;
	return;
}

Client::~Client()
{
	if (_pWebsocket) {
        _pWebsocket->abort();
		_pWebsocket = 0;
	}
    TempTimer->stop();
    state = 0;  //退出程序断开连接
}

void Client::CreateWebscoketlink()
{
	if (_pWebsocket == Q_NULLPTR) {
		_pWebsocket = new QWebSocket();
		_pWebsocket->open(QUrl(ServerUrl));
        OnServerTip->show();
		connect(_pWebsocket, SIGNAL(connected()), this, SLOT(OnConnected()), Qt::AutoConnection);   //建立连接
		connect(m_timer, SIGNAL(timeout()), this, SLOT(reconnect()), Qt::AutoConnection);   //到达等待时间，进行重连
		connect(_pWebsocket, SIGNAL(textMessageReceived(QString)), this, SLOT(OntextRec(QString)), Qt::AutoConnection);  //返回收到的json语句
		connect(_pWebsocket, SIGNAL(disconnected()), this, SLOT(OnDisconnected()), Qt::AutoConnection);   //断开连接时，进行计时
        qDebug() << "create _pwebsocket succeed!" << endl;
	}
}

void Client::OnConnected()
{
    m_timer->stop();
    OnServerTip->hide();
    QMessageBox::information(NULL,"提示","与云服务器连接成功，请点击下方开机按钮进入控制界面","确认");
    //使控件能够点击
    w->all_enabled();

    startflag = 1;

    state = 1;           //已连接状态
    totalcount = 5;
	qDebug() << "Address: " << _pWebsocket->peerAddress() << endl;

    //开始连接时，实时进行温度变化监控
    TempTimer->start(94);   //每5s更新一次室内温度
}

void Client::OntextRec(QString strData)
{
	qDebug() << "获取的消息内容： " << strData << endl;
	QJsonParseError parseJsonErr;
	QJsonDocument document = QJsonDocument::fromJson(strData.toUtf8(), &parseJsonErr);
	if (!(parseJsonErr.error == QJsonParseError::NoError)) {
		qDebug() << "json解析错误" << endl;
		return;
	}
	QJsonObject jsonObject = document.object();

    int modify = 0;   //未修改

    //未开机时进行除tem以外的报文过滤
    if(status == 0){
        if (jsonObject.contains(QStringLiteral("event"))) {
            QString Recevent = jsonObject["event"].toString();
            if(Recevent != "tem" && Recevent != "on")
                return;
        }
    }

    //主机发送消息的解析
    if (jsonObject["status"].toInt())
    {
        //判断房间号是否一致
        QString Room_id = "";
        if (jsonObject.contains(QStringLiteral("event"))) {
            Room_id = jsonObject["room_id"].toString();
            //如果不是给该房间的报文，则忽略之
            if(Room_id != QString::number(room_id)){
                return ;
            }
        }
        //获取事件类型
        QString Recevent = "", attention = "";
        if (jsonObject.contains(QStringLiteral("event"))) {
            Recevent = jsonObject["event"].toString();
        }
        //弹窗，连接失败，请稍后再尝试
        if (Recevent == "on") {
            attention =  QString("请求服务器开机失败，目前按照默认方式工作，请稍后再次尝试");
            status = 0;    //开机失败
        }
        else if (Recevent == "off") {
            attention =  QString("请求服务器关机失败，目前按照默认方式工作，请稍后再次尝试");
            status = 1;    //关机失败
        }
        else if (Recevent == "set") {
            tar_temp = last_tar_temp;
            w->change_tar_temp(tar_temp);

            windlevel = last_wind_level;
            w->change_wind(windlevel);
            attention =  QString("请求服务器调整温度或风速失败，目前按照默认方式工作，请稍后再次尝试");
        }
        QMessageBox::critical(NULL, ("温馨提示"), "%s",attention);
    }
    else{
        //获取事件类型
        QString Recevent = "", attention = "";
        //客户端的变动成功生效
        if (jsonObject.contains(QStringLiteral("event"))) {
            Recevent = jsonObject["event"].toString();
            qDebug() << "Recvent = " << Recevent << endl;
        }
        if(Recevent == "on"){
            status = 1;
            //判断是否可调度
            if(jsonObject.contains(QStringLiteral("data")))
            {
                QJsonValue value = jsonObject.value("data");
                if (value.isObject()) {
                    QJsonObject obj = value.toObject();
                    int windset;
                    if (obj.contains("wind")) {
                        windset = obj.value("wind").toInt();
                        //调度离开
                        if(windset == 0){
                            depart = 0;
                        }
                        //调度开始
                        else
                            depart = 1;
                    }
                }
            }
        }
        if (Recevent == "set") {
            if(jsonObject.contains(QStringLiteral("data")))
            {
                QJsonValue value = jsonObject.value("data");
                if (value.isObject()) {
                    QJsonObject obj = value.toObject();
                    int windset;
                    if (obj.contains("wind")) {
                        windset = obj.value("wind").toInt();
                        //调度离开
                        if(windset == windlevel){
                            depart = 1;
                        }
                        //调度继续
                        else
                            depart = 0;
                    }
                }
            }
            //在调度时，才改变参数
            if(depart != 0){
                if(powersave == 0){
                    last_wind_level_power_saving = windlevel;
                    last_wind_level = windlevel;
                }
                w->change_wind(windlevel);

                last_tar_temp = tar_temp;
                w->change_tar_temp(tar_temp);
            }
        }
        if(Recevent == "tem"){
            modify = 1;
        }
        if(Recevent == "off"){
            status = 0;
        }
    }
	//获取各种类型的参数
    if (jsonObject.contains(QStringLiteral("data"))) {
		QJsonValue value = jsonObject.value("data");
		if (value.isObject()) {
			QJsonObject obj = value.toObject();
			//获取累计金额值
            if (obj.contains("unit_fee")) {
                //更新单价
                unit_fee = (obj.value("unit_fee").toDouble());
                w->update_unifee(unit_fee);
			}
            if (obj.contains("tot_fee")) {
                //更新总价
                fee = (obj.value("tot_fee").toDouble());
                w->update_total_money(fee);
            }
            if (obj.contains("cur_fee")) {
                //更新本次费用
                cur_fee = (obj.value("cur_fee").toDouble());
                w->update_current_money(cur_fee);
            }
            if (obj.contains("mode") && modify == 1) {
                //更新工作模式
                mode = obj.value("mode").toInt();
                w->change_mode(mode);
            }
		}
	}
}


void Client::OnDisconnected()
{
	qDebug() << "Failed to connected with server!" << endl;

    oncount = 1;
    status = 0;  //连接断开视为关机
    //csy:ui断开连接显示 4.6
    w->all_disabled();

	if (totalcount > 0) {
		totalcount--;
	}
	else {
        state = 0;
		//弹窗，连接失败，启动异常，请稍后再尝试
        QMessageBox::warning(NULL,  ("温馨提示"),  ("服务器连接失败，重连次数超限，请稍后再试"),  ("确认"));
		qApp->quit();   //连接次数超过五次后，停机
	}
    TempTimer->stop();
    if(state == 1)   //异常才重连
        m_timer->start(3000);   //断开,计时器开始计时
}

void Client::reconnect()
{
	qDebug() << "websocket reconnected";
	_pWebsocket->abort();
	_pWebsocket->open((QUrl(ServerUrl)));

    QMessageBox *m_box = new QMessageBox(QMessageBox::Information,  ("通知"),  ("第") + QString::number(5 - totalcount) +  ("次服务器重新连接中"));
	m_box->addButton(QMessageBox::Ok);
	m_box->button(QMessageBox::Ok)->hide();
	QTimer::singleShot(3000, m_box, SLOT(accept()));
	m_box->exec();
}
