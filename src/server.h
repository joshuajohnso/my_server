#pragma once
#include "ui_server.h"
#include <QWidget>
#include <QTcpServer> //服务端
#include <QTcpSocket> //实现tcp请求
#include <QDateTime> //获取系统时间
#include <QMessageBox>
#include <QMap>

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
    void boardcastMessage(QTcpSocket *sender, const QString &message); //广播消息给所有连接的客户端

private:
    Ui_server* ui;
    QTcpServer *TcpServer; //tcp服务端对象
    // QTcpSocket *TcpSocket; //tcp套接字对象
    QList<QTcpSocket*> TcpSockets; //支持多个客户端连接,使用列表存储Socket对象
    QDateTime CurrentTime; //系统时间对象
    QMap<QTcpSocket*, QString> socketUserMap;
    QMap<QTcpSocket*, QByteArray> serverRecvMap;  // IMAGE 分包缓冲
    void displayEmoji(const QString &username, const QString &emojiFile);
    void displayImage(const QString &username, const QByteArray &imageData);
};