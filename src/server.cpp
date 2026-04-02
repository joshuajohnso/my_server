#include "server.h"

server::server(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui_server)
    , TcpServer(new QTcpServer(this)) //初试化服务器
    , TcpSocket(nullptr) //初始表示未连接
{
    ui->setupUi(this);

    //监听任意IP地址的8080端口
    if(!TcpServer->listen(QHostAddress::Any, 8080)) {
        QMessageBox::critical(this, "Error", "Unable to start the server: " + TcpServer->errorString()); //提供错误弹窗以及错误信息
        // close();
        return;
    }

    //连接新连接信号到槽函数
    connect(TcpServer, &QTcpServer::newConnection, this, &server::onNewConnection);
    
    //发送按钮信息
    // connect(ui->Send, &QPushButton::clicked, this, &server::on_Send_clicked); //on_Send_clicked = 转到槽 (因为控件就叫Send)
    // connect(ui->Close, &QPushButton::clicked, this, &server::on_Close_clicked);

    connect(TcpSocket, &QTcpSocket::disconnected, this, &server::onClientDisconnected); //清理套接字

}

void server::onNewConnection(){
    CurrentTime = QDateTime::currentDateTime(); //获取当前系统时间
    ui->Read->append("收到连接信息:" + CurrentTime.toString("yyyy年  M月  d日  HH:mm:ss")); //在Read文本框显示连接时间
    TcpSocket = TcpServer->nextPendingConnection(); //获取新连接的套接字
    connect(TcpSocket, &QTcpSocket::readyRead, this, &server::onDataReceived); //连接数据接收信号到槽函数
}

void server::onDataReceived() {
    QByteArray data = TcpSocket->readAll(); //读取数据
    ui->Read->append("收到数据:" + QString::fromUtf8(data)); //在Read文本框显示收到的客户端数据
}

void server::onClientDisconnected() {
    ui->Read->append("客户端已断开连接"); //在Read文本框显示客户端断开连接
    TcpSocket = nullptr; //重置套接字指针
}

void server::on_Send_clicked(){
    if(TcpSocket) {
        QString message = ui->Write->toPlainText(); //获取TextEdit的内容
        TcpSocket->write(message.toUtf8()); //发送数据
        // ui->Read->append("发送数据:" + message); //在Read文本框显示发送的数据
    } else {
        QMessageBox::warning(this, "Warning", "没有连接到客户端!"); //如果没有连接到客户端，弹出警告信息
    }
}

void server::on_Close_clicked(){
    if(TcpSocket) {
        TcpSocket->disconnectFromHost(); //断开连接
        ui->Read->append("连接已关闭"); //在Read文本框显示连接已关闭
        TcpSocket = nullptr; //重置套接字指针
    } else {
        QMessageBox::warning(this, "Warning", "没有连接到客户端!"); //如果没有连接到客户端，弹出警告信息
    }
}

server::~server()
{
    delete ui; 
}