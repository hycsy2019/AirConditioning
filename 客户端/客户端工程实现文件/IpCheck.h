#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QString>

class LoginForm : public QDialog
{
    Q_OBJECT //使用信号与槽需要的宏
public:
    explicit LoginForm(QDialog *parent = 0);  //explicit 防止歧义
public slots:
    void login();//点击登录按钮是执行的槽函数
private:
    QLabel *LabelServerIp;         //"服务器ip"标签
    QLabel *LabelServerPort;              //"端口"标签
    QLabel *LabelRoomid;         //"服务器ip"标签
    QLabel *LabelCurtemp;              //"端口"标签
    QLabel *LabelTartemp;         //"服务器ip"标签
    QLabel *LabelWindlevel;              //"端口"标签
public:
    QLineEdit *ip;              //ip
    QLineEdit *port;             //端口
    QLineEdit *roomid;           //房间号
    QLineEdit *curtemp;          //房间温度
    QLineEdit *tartemp;           //目标温度
    QLineEdit *windlevel;         //风速
    QPushButton *loginBtn;       //登录按钮
    QPushButton *exitBtn;        //退出按钮
};

#endif // LOGINFORM_H
