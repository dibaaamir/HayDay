#include "farm.h"
#include <QJsonArray>
#include <QJsonDocument>
#include "globals.h"
#include <QDateTime>

int Farm::id_ = 0;
Farm *Farm::farm = nullptr;

Farm &Farm::get(int farm_id)
{
    if (farm == nullptr)
        farm = new Farm;

    if (farm_id != farm->id_)
    {
        farm->id_ = farm_id;

        QString query = "SELECT barn_id, silo_id, alfalfa_field_id, wheat_field_id FROM Farms WHERE id=:id";
        query.replace(":id", QString::number(farm_id));

        socket.write(query);
        QJsonDocument servers_answer = QJsonDocument::fromJson(socket.read());

        if (!servers_answer.isNull())
        {
            QJsonArray data = servers_answer.array()[0].toArray();

            farm->barn_id_ = data[0].toString().toInt();
            farm->silo_id_ = data[1].toString().toInt();
            farm->alfalfa_field_id_ = data[2].toString().toInt();
            farm->wheat_field_id_ = data[3].toString().toInt();
        }
        else
        {
            QString err = "Farm with id " + QString::number(farm_id) + " doesn't exist!";
            throw err.toStdString().c_str();
        }
    }

    return *farm;
}

Farm &Farm::create(int barn_id, int silo_id, int alfalfa_field_id, int wheat_field_id)
{
    if (farm != nullptr)
        delete farm;

    farm = new Farm;
    farm->barn_id_ = barn_id;
    farm->silo_id_ = silo_id;
    farm->alfalfa_field_id_ = alfalfa_field_id;
    farm->wheat_field_id_ = wheat_field_id;

    QString query = "INSERT INTO Farms(barn_id, silo_id, alfalfa_field_id, wheat_field_id) VALUES(:barn_id, :silo_id, :alfalfa_field_id, :wheat_field_id)";
    query.replace(":barn_id", QString::number(barn_id));
    query.replace(":silo_id", QString::number(silo_id));
    query.replace(":alfalfa_field_id", QString::number(alfalfa_field_id));
    query.replace(":wheat_field_id", QString::number(wheat_field_id));

    socket.write(query);
    id_ = socket.read().toInt();

    return *farm;
}
