#include "server.h"
#include "ui_server.h"

Server::Server(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Server)
{
    ui->setupUi(this);
}

Server::~Server()
{
    delete ui;
}

void Server::startServer()
{
    allClients = new QVector<QTcpSocket*>;

    server = new QTcpServer();
    server->setMaxPendingConnections(5);

    connect(server,SIGNAL(newConnection()),this,SLOT(newClientConnection()));

    if(server->listen(QHostAddress::Any,8001))
    {
        qDebug()<<"Сервер запущен, порт 8001";
    }
    else
    {
        qDebug()<<"Сервер не запущун. Ошибка:"+server->errorString();
    }
}

void Server::sendMessageToClients(QString message)
{
    if(allClients->size()>0)
    {
        for (int i =0;i<allClients->size();i++) {
            if(allClients->at(i)->isOpen() && allClients->at(i)->isWritable())
            {
                allClients->at(i)->write(message.toUtf8());
            }
        }
    }
}

void Server::newClientConnection()
{
    QTcpSocket *client = server->nextPendingConnection();
    QString ipAddress = client->peerAddress().toString();
    int port = client->peerPort();

    connect(client,&QTcpSocket::disconnected,this,&Server::socketDisconnected);
    connect(client,&QTcpSocket::readyRead,this,&Server::socketReadyRead);
    connect(client,&QTcpSocket::stateChanged,this,&Server::socketStateChanged);

    allClients->push_back(client);
    qDebug()<<"Сокет подключился к "+ipAddress+":"+QString::number(port);

}

void Server::socketDisconnected()
{
    QTcpSocket *client = qobject_cast<QTcpSocket*>(QObject::sender());
    QString socketIpAddress = client->peerAddress().toString();
    int port=client->peerPort();

    qDebug()<<"Сокет отключился от "+socketIpAddress+":"+QString::number(port);
}

void Server::socketReadyRead()
{
    QTcpSocket* client = qobject_cast<QTcpSocket*>(QObject::sender());
    QString socketIpAddress = client->peerAddress().toString();
    int port = client->peerPort();
    QString data = QString(client->readAll());
    qDebug()<<"Сообщение: "+data+" (" +socketIpAddress+":"+QString::number(port)+")";

    sendMessageToClients(data);
}

void Server::socketStateChanged(QAbstractSocket::SocketState state)
{
    QTcpSocket* client = qobject_cast<QTcpSocket*>(QObject::sender());
    QString socketIpAddress = client->peerAddress().toString();
    int port = client->peerPort();
    QString desc;
    if (state == QAbstractSocket::UnconnectedState)
        desc = "Сокет не подключен.";
    else if (state == QAbstractSocket::HostLookupState)
        desc = "Поиск имени хоста.";
    else if (state == QAbstractSocket::ConnectingState)
        desc = "Сокет начал устанавливать соединение.";
    else if (state == QAbstractSocket::ConnectedState)
        desc = "Соединение установлено.";
    else if (state == QAbstractSocket::BoundState)
        desc = "Сокет привязан к адресу и порту.";
    else if (state == QAbstractSocket::ClosingState)
        desc = "Сокет вот-вот закроется (возможно, данные ожидают записи).";
    else if (state == QAbstractSocket::ListeningState)
        desc = "Только для внутреннего использования.";
    qDebug()<<"Состояние сокета изменено ("+socketIpAddress+":"+QString::number(port)+"):"+desc;
}

