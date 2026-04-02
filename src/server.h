#pragma once
#include "ui_server.h"
#include <QWidget>
#include <QTcpServer> //服务端
#include <QTcpSocket> //实现tcp请求
#include <QDateTime> //获取系统时间
#include <QMessageBox>

class server : public QWidget {
    Q_OBJECT
    
public:
    server(QWidget* parent = nullptr);
    ~server();

private slots:
    void on_Send_clicked(); //发送按钮的槽函数
    void on_Close_clicked(); //关闭按钮的槽函数
    void onNewConnection(); //新连接的槽函数
    void onDataReceived(); //数据接收的槽函数
    void onClientDisconnected(); //客户端断开连接的槽函数

private:
    Ui_server* ui;
    QTcpServer *TcpServer; //tcp服务端对象
    QTcpSocket *TcpSocket; //tcp套接字对象
    QDateTime CurrentTime; //系统时间对象
};