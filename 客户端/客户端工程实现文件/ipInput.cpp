#include "IpCheck.h"

//临时参数值
QString str_ip;              //ip
QString str_port;             //端口
QString str_roomid;           //房间号
QString str_curtemp;          //房间温度
QString str_tartemp;           //目标温度
QString str_windlevel;         //风速

LoginForm::LoginForm(QDialog *parent) :
    QDialog(parent)
{
    //设置背景色
    QPalette  palette (this->palette());
    palette.setColor(QPalette::Background, Qt::gray);
    this->setPalette( palette );

    //设置窗体标题
    this->setWindowTitle(tr("初始参数设置界面"));
    this->resize(600,500);
    //ip
    LabelServerIp = new QLabel(this);   //new一个标签对象
    LabelServerIp->move(0,80);         //移动到(70,80)位置(Label左上角坐标，相对于父窗体)
    LabelServerIp->setText("服务器IP");  //设置标签文本
    LabelServerIp->setStyleSheet("QLabel{border:2px solid rgb(200, 255, 255);}");

    //port
    LabelServerPort = new QLabel(this);
    LabelServerPort->move(0,130);
    LabelServerPort->setText("端口号");//占位符
    LabelServerPort->setStyleSheet("QLabel{border:2px solid rgb(200, 255, 255);}");

    //房间号
    LabelRoomid = new QLabel(this);
    LabelRoomid->move(0,180);
    LabelRoomid->setText("房间号");
    LabelRoomid->setStyleSheet("QLabel{border:2px solid rgb(200, 255, 255);}");

    //室温
    LabelCurtemp = new QLabel(this);
    LabelCurtemp->move(0,230);
    LabelCurtemp->setText("室温设定");
    LabelCurtemp->setStyleSheet("QLabel{border:2px solid rgb(200, 255, 255);}");

    //目标温度
    LabelTartemp = new QLabel(this);
    LabelTartemp->move(0,280);
    LabelTartemp->setText("目标温度设定");
    LabelTartemp->setStyleSheet("QLabel{border:2px solid rgb(200, 255, 255);}");

    //风速
    LabelWindlevel = new QLabel(this);
    LabelWindlevel->move(0,320);
    LabelWindlevel->setText("风速设定");
    LabelWindlevel->setStyleSheet("QLabel{border:2px solid rgb(200, 255, 255);}");

    //ip
    ip = new QLineEdit(this);
    ip->move(150,80);
    ip->setPlaceholderText("请输入ip");
    ip->setStyleSheet("QLineEdit{border-width:6px;border-radius:4px;font-size:22px;color:black;border:2px solid gray;}"
                                "QLineEdit:hover{border-width:4px;border-radius:4px;font-size:12px;color:black;border:1px solid rgb(70,200,50);}");

    ip->setText("10.128.209.15");
    //port
    port = new QLineEdit(this);
    port->move(150,130);
    port->setPlaceholderText("请输入端口号");
    port->setStyleSheet("QLineEdit{border-width:6px;border-radius:4px;font-size:22px;color:black;border:2px solid gray;}"
                        "QLineEdit:hover{border-width:4px;border-radius:4px;font-size:12px;color:black;border:1px solid rgb(70,200,50);}");

    port->setText("80");
    //roomid
    roomid = new QLineEdit(this);
    roomid->move(150,180);
    roomid->setPlaceholderText("请输入房间号");
    roomid->setStyleSheet("QLineEdit{border-width:6px;border-radius:4px;font-size:22px;color:black;border:2px solid gray;}"
                          "QLineEdit:hover{border-width:4px;border-radius:4px;font-size:12px;color:black;border:1px solid rgb(70,200,50);}");

    roomid->setText("101");
    //室温
    curtemp = new QLineEdit(this);
    curtemp->move(150,230);
    curtemp->setPlaceholderText("请输入室温");
    curtemp->setStyleSheet("QLineEdit{border-width:6px;border-radius:4px;font-size:22px;color:black;border:2px solid gray;}"
                           "QLineEdit:hover{border-width:4px;border-radius:4px;font-size:12px;color:black;border:1px solid rgb(70,200,50);}");

    curtemp->setText("28");
    //目标温度
    tartemp = new QLineEdit(this);
    tartemp->move(150,280);
    tartemp->setPlaceholderText("请输入目标温度");
    tartemp->setStyleSheet("QLineEdit{border-width:6px;border-radius:4px;font-size:22px;color:black;border:2px solid gray;}"
                           "QLineEdit:hover{border-width:4px;border-radius:4px;font-size:12px;color:black;border:1px solid rgb(70,200,50);}");

    tartemp->setText("25");
    //密码输入框
    windlevel = new QLineEdit(this);
    windlevel->move(150,320);
    windlevel->setPlaceholderText("请输入风速");
    windlevel->setStyleSheet("QLineEdit{border-width:6px;border-radius:4px;font-size:22px;color:black;border:2px solid gray;}"
                             "QLineEdit:hover{border-width:4px;border-radius:4px;font-size:12px;color:black;border:1px solid rgb(70,200,50);}");

    windlevel->setText("2");
    //登录按钮
    loginBtn = new QPushButton(this);
    loginBtn->move(150,430);
    loginBtn->setText("确认");
    loginBtn->setStyleSheet(
                //正常状态样式
                "QPushButton{"
                "background-color:rgba(100,225,100,30);"//背景色（也可以设置图片）
                "border-style:outset;"                  //边框样式（inset/outset）
                "border-width:4px;"                     //边框宽度像素
                "border-radius:10px;"                   //边框圆角半径像素
                "border-color:rgba(255,255,255,30);"    //边框颜色
                "font:bold 20px;"                       //字体，字体大小
                "color:rgba(0,0,0,100);"                //字体颜色
                "padding:20px;"                          //填衬
                "}"
                //鼠标按下样式
                "QPushButton:pressed{"
                "background-color:rgba(100,255,100,200);"
                "border-color:rgba(255,255,255,30);"
                "border-style:inset;"
                "color:rgba(0,0,0,100);"
                "}"
                //鼠标悬停样式
                "QPushButton:hover{"
                "background-color:rgba(100,255,100,100);"
                "border-color:rgba(255,255,255,200);"
                "color:rgba(0,0,0,200);"
                "}");

    //退出按钮
    exitBtn = new QPushButton(this);
    exitBtn->move(300,430);
    exitBtn->setText("退出");
    exitBtn->setStyleSheet(
                //正常状态样式
                "QPushButton{"
                "background-color:rgba(100,225,100,30);"//背景色（也可以设置图片）
                "border-style:outset;"                  //边框样式（inset/outset）
                "border-width:4px;"                     //边框宽度像素
                "border-radius:10px;"                   //边框圆角半径像素
                "border-color:rgba(255,255,255,30);"    //边框颜色
                "font:bold 20px;"                       //字体，字体大小
                "color:rgba(0,0,0,100);"                //字体颜色
                "padding:20px;"                          //填衬
                "}"
                //鼠标按下样式
                "QPushButton:pressed{"
                "background-color:rgba(100,255,100,200);"
                "border-color:rgba(255,255,255,30);"
                "border-style:inset;"
                "color:rgba(0,0,0,100);"
                "}"
                //鼠标悬停样式
                "QPushButton:hover{"
                "background-color:rgba(100,255,100,100);"
                "border-color:rgba(255,255,255,200);"
                "color:rgba(0,0,0,200);"
                "}");

    //单击登录按钮时 执行 LoginForm::login 槽函数(自定义)；单击退出按钮时 执行 LoginForm::close 槽函数(窗体的关闭函数，不用自己写)
    connect(loginBtn,&QPushButton::clicked,this,&LoginForm::login);
    connect(exitBtn,&QPushButton::clicked,this,&LoginForm::close);

}

void LoginForm::login()
{
    str_ip = ip->text();
    str_port = port->text();
    str_roomid = roomid->text();
    str_curtemp = curtemp->text();
    str_tartemp = tartemp->text();
    str_windlevel = windlevel->text();
    accept();
}
