#include "channel.h"
#include <QSqlQuery>

int Channel::login(const QString& username, const QString& password)
{
    QSqlDatabase db = QSqlDatabase::database(QString::number(socketDescriptor));

    QSqlQuery query(db);
    query.prepare("SELECT id FROM Accounts WHERE username=:username AND password=:password");
    query.bindValue(":username", username);
    query.bindValue(":password", password);
    query.exec();

    if (query.first())
    {
        int account_id = query.value(0).toInt();
        query.clear();
        query.prepare("SELECT id FROM Farmers WHERE account_id=:account_id");
        query.bindValue(":account_id", account_id);
        query.exec();
        query.first();
        return query.value(0).toInt();
    }

    return 0;
}

Channel::Channel(qintptr ID, QObject *parent) : QThread(parent)
{
    socketDescriptor = ID;

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", QString::number(socketDescriptor));
    db.setDatabaseName("../database/HayDayDB.sqlite");
    db.open();
}

Channel::~Channel()
{
    QSqlDatabase db = QSqlDatabase::database(QString::number(socketDescriptor));
    db.removeDatabase(QString::number(socketDescriptor));
    db.close();
}

void Channel::run()
{
    qDebug() << " Thread started";
    socket = new QTcpSocket();
    if(!socket->setSocketDescriptor(this->socketDescriptor))
    {
        emit error(socket->error());
        return;
    }
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    qDebug() << socketDescriptor << " Client connected";
    exec();
}

void Channel::readyRead()
{
    QString data = socket->readAll();
    if(data.startsWith("Login"))
    {
        data.remove("Login(");
        QString username = data.split(',')[0];
        QString password = data.split(',')[1];
        password.chop(1);
        qDebug() << username << password;

        data.setNum(login(username, password));
        socket->write(data.toUtf8());
    }
}

void Channel::disconnected()
{
    qDebug() << socketDescriptor << " Disconnected";
    socket->deleteLater();
    exit(0);
}