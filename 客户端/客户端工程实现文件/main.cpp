#include "control_panel.h"
#include "IpCheck.h"
#include <QtWidgets/QApplication>
#include <QDebug>

MainWindow *w;
Client *client;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    w=new MainWindow();
    w->show();
    return a.exec();
}
