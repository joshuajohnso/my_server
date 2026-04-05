#include "server.h"

server::server(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui_server)
    , TcpServer(new QTcpServer(this)) //初试化服务器
    , TcpSockets() //初始化套接字列表
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

}

void server::onNewConnection(){
    CurrentTime = QDateTime::currentDateTime(); //获取当前系统时间
    ui->Read->append("收到连接信息:" + CurrentTime.toString("yyyy-MM-dd hh:mm:ss")); //在Read文本框显示连接时间
    QTcpSocket *newSocket = TcpServer->nextPendingConnection(); //获取新连接的套接字
    TcpSockets.append(newSocket); //将新套接字添加到列表中
    connect(newSocket, &QTcpSocket::readyRead, this, &server::onDataReceived); //连接数据接收信号到槽函数
    connect(newSocket, &QTcpSocket::disconnected, this, &server::onClientDisconnected); //连接断开信号到槽函数
}

void server::onDataReceived() {
    QTcpSocket *TcpSocket = qobject_cast<QTcpSocket*>(sender());

    if (serverRecvMap.contains(TcpSocket)) {
        serverRecvMap[TcpSocket] += TcpSocket->readAll();
        if (!serverRecvMap[TcpSocket].contains('\n')) return;
        int idx = serverRecvMap[TcpSocket].indexOf('\n');
        QByteArray fullData = serverRecvMap[TcpSocket].left(idx);
        serverRecvMap.remove(TcpSocket);

        QString text     = QString::fromUtf8(fullData);
        QString username = text.section('|', 1, 1);
        QByteArray raw   = QByteArray::fromBase64(text.section('|', 2, 2).toUtf8());
        displayImage(username, raw);                                   // 显示在服务端
        boardcastMessage(TcpSocket, text + "\n");                      // 广播给其他客户端
        return;
    }

    QString data = QString::fromUtf8(TcpSocket->readAll());

    if (data.startsWith("LOGIN|")) {
        QString username = data.section('|', 1, 1);
        socketUserMap[TcpSocket] = username;  // 记录 socket 对应的用户名
        ui->Read->append("用户 " + username + " 已连接");
    } else if (data.startsWith("LOGOUT|")) {
        QString username = data.section('|', 1, 1);
        ui->Read->append("用户 " + username + " 请求断开");
        socketUserMap.remove(TcpSocket);
    } else if(data.startsWith("EMOJI|")){
        QString username  = data.section('|', 1, 1);
        QString emojiFile = data.section('|', 2, 2);
        displayEmoji(username, emojiFile);
        boardcastMessage(TcpSocket, data);
    } else if (data.startsWith("IMAGE|")) {
        if (data.endsWith('\n')) {
            QString username = data.section('|', 1, 1);
            QByteArray raw   = QByteArray::fromBase64(data.section('|', 2, 2).toUtf8());
            displayImage(username, raw);                               // 显示在服务端
            boardcastMessage(TcpSocket, data);         
        } else {
            serverRecvMap[TcpSocket] = data.toUtf8();  // 未收完，缓存等待
        }
    }
    else {
        ui->Read->append(data);
        boardcastMessage(TcpSocket, data);
    }
}

void server::onClientDisconnected() {
    QTcpSocket *TcpSocket = qobject_cast<QTcpSocket*>(sender());

    // 查 map 里有没有这个 socket 对应的用户名(异常退出判断)
    if (socketUserMap.contains(TcpSocket)) {
        QString username = socketUserMap.value(TcpSocket);
        ui->Read->append("用户 " + username + " 异常断开");
        socketUserMap.remove(TcpSocket);
    }
    
    TcpSockets.removeOne(TcpSocket);
    TcpSocket->deleteLater();
}

// void server::onDataReceived() {
//     QTcpSocket *TcpSocket = qobject_cast<QTcpSocket*>(sender()); //获取发送信号的套接字
//     QByteArray data = TcpSocket->readAll(); //读取数据
//     ui->Read->append(QString::fromUtf8(data)); //在Read文本框显示收到的客户端数据
//     boardcastMessage(TcpSocket, QString::fromUtf8(data)); //广播消息给所有客户端
// }

// void server::onClientDisconnected() {
//     QTcpSocket *TcpSocket = qobject_cast<QTcpSocket*>(sender()); //获取发送信号的套接字
//     TcpSockets.removeOne(TcpSocket); //从列表中移除断开的套接字
//     TcpSocket->deleteLater(); //删除套接字
// }

void server::on_Send_clicked(){
    if(TcpSockets.isEmpty()) {
        QMessageBox::warning(this, "Warning", "没有连接到客户端!"); //如果没有连接到客户端，弹出警告信息
        return;
    }
    QString msg = ui->Write->toPlainText(); //获取TextEdit的内容
    // 使用带格式的消息字符串
    QString serverName = QString("<b style='color:green'>服务器</b>"); //服务器名称
    QString message = QString("[%1] %2: %3").arg(QDateTime::currentDateTime().toString("hh:mm:ss")).arg(serverName).arg(msg);
    for(QTcpSocket *Tcpsocket : TcpSockets) {
        if(Tcpsocket->state() == QAbstractSocket::ConnectedState) {            
            Tcpsocket->write(message.toUtf8()); //给所有客户端发送数据
        }
    }
    ui->Read->append(message); //在Read文本框显示发送的数据
    ui->Write->clear(); //清空TextEdit
} 


void server::on_Close_clicked(){
    if(TcpSockets.isEmpty()) {
        QMessageBox::warning(this, "Warning", "没有连接到客户端!"); //如果没有连接到客户端，弹出警告信息
    } 
    else {
        for (QTcpSocket *socket : TcpSockets) {
            if (socket->state() == QAbstractSocket::ConnectedState) {
                socket->disconnectFromHost(); //断开连接
            }
        }
        ui->Read->append("连接已关闭"); //在Read文本框显示连接已关闭
        TcpSockets.clear(); //清空套接字列表
    }
}

//屏蔽发送者广播
void server::boardcastMessage(QTcpSocket *sender, const QString &message) {
    for (QTcpSocket *Tcpsocket : TcpSockets) {
        //不发送给消息的发送者
        if(Tcpsocket != sender && Tcpsocket->state() == QAbstractSocket::ConnectedState) {            
            Tcpsocket->write(message.toUtf8()); //发送数据
        }
    }
}

void server::displayEmoji(const QString &username, const QString &emojiFile)
{
    QTextCursor cursor = ui->Read->textCursor();
    cursor.movePosition(QTextCursor::End);
     // 若当前行不为空则先换行，避免表情紧跟在上一条消息后面
    if (!cursor.atBlockStart()) {
        cursor.insertBlock();
    }
    cursor.insertText(QString("[%1] %2: ").arg(QDateTime::currentDateTime().toString("hh:mm:ss")).arg(username));

    QImage img(QString(":/emojis/%1").arg(emojiFile));
    QString key = "emoji_" + emojiFile;
    ui->Read->document()->addResource(QTextDocument::ImageResource, QUrl(key), img);

    QTextImageFormat fmt;
    fmt.setName(key);
    fmt.setWidth(32);
    fmt.setHeight(32);
    cursor.insertImage(fmt);
    cursor.insertText("\n");
    ui->Read->setTextCursor(cursor);
}

void server::displayImage(const QString &username, const QByteArray &raw)
{
    static int imgIndex = 0;
    QImage img;
    img.loadFromData(raw);

    QString key = QString("img_%1").arg(imgIndex++);
    ui->Read->document()->addResource(QTextDocument::ImageResource, QUrl(key), img);

    QTextCursor cursor = ui->Read->textCursor();
    cursor.movePosition(QTextCursor::End);
    if (!cursor.atBlockStart()) {
        cursor.insertBlock();
    }
    cursor.insertText(QString("[%1] %2 :\n").arg(QDateTime::currentDateTime().toString("hh:mm:ss")).arg(username));

    QTextImageFormat fmt;
    fmt.setName(key);
    fmt.setWidth(180);  // 显示缩略图宽度
    cursor.insertImage(fmt);
    cursor.insertText("\n");
    ui->Read->setTextCursor(cursor);
}

server::~server()
{
    delete ui; 
}