#include "barn.h"
#include <QJsonObject>
#include <QJsonDocument>
#include "globals.h"
#include <QDateTime>

int Barn::id_ = 0;
Barn *Barn::barn = nullptr;

Barn::Barn()
{
    storage_ = 1;
    max_storage_ = 5;
    shovels_ = 0;
    nails_ = 1;
    alfalfas_ = 0;
    eggs_ = 0;
    wools_ = 0;
    upgrade_day_ = -1;
    is_upgrading_ = false;
    level_ = 1;
}

Barn &Barn::get(int barn_id)
{
    if (barn == nullptr)
        barn = new Barn;

    if (barn_id != barn->id_)
    {
        barn->id_ = barn_id;
        barn->milks_.clear();

        QString query = "SELECT storage, max_storage, shovels, nails, alfalfas, eggs, wools, upgrade_day, "
                        "is_upgrading, level FROM Barns WHERE id=:id";
        query.replace(":id", QString::number(barn_id));

        socket.write(query);
        QJsonDocument servers_answer = QJsonDocument::fromJson(socket.read());

        if (!servers_answer.isNull())
        {
            QJsonObject json_obj = servers_answer.object();
            barn->storage_ = json_obj["0"].toInt();
            barn->max_storage_ = json_obj["1"].toInt();
            barn->shovels_ = json_obj["2"].toInt();
            barn->nails_ = json_obj["3"].toInt();
            barn->alfalfas_ = json_obj["4"].toInt();
            barn->eggs_ = json_obj["5"].toInt();
            barn->wools_ = json_obj["6"].toInt();
            barn->upgrade_day_ = json_obj["7"].toInt();
            barn->is_upgrading_ = json_obj["8"].toInt();
            barn->level_ = json_obj["9"].toInt();

            //            query = "SELECT id, manufacture_day, expiration_day"
            //                          " WHERE barn_id=:barn_id";
            //            query.replace(":barn_id", QString::number(barn_id));

            //            socket.write(query);
            //            QJsonDocument servers_answer = QJsonDocument::fromJson(socket.read());
            //            Milk milk(barn_id);
            //            while (query.next())
            //            {
            //                milk.setId(query.value(0).toInt());
            //                milk.setManufactureDay(query.value(1).toInt());
            //                milk.setExpirationDay(query.value(0).toInt());

            //                barn->milks_.push_back(milk);
            //            }
        }
        else
        {
            std::string err = "Barn with id " + std::to_string(barn_id) + " doesn't exist!";
            throw std::exception(err.c_str());
        }
    }

    return *barn;
}

Barn &Barn::create()
{
    if (barn != nullptr)
        delete barn;

    barn = new Barn;
    QString query = "INSERT INTO Barns DEFAULT VALUES";

    socket.write(query);
    id_ = socket.read().toInt();
    return *barn;
}

void Barn::save() const
{
    QString query = "UPDATE Barns SET storage=:storage, max_storage=:max_storage, "
                    "shovels=:shovels, nails=:nails, alfalfas=:alfalfas, eggs=:eggs, wools=:wools, "
                    "upgrade_day=:upgrade_day, is_upgrading=:is_upgrading, level=:level "
                    "WHERE id=:id";

    query.replace(":storage", QString::number(id_));
    query.replace(":max_storage", QString::number(max_storage_));
    query.replace(":shovels", QString::number(shovels_));
    query.replace(":nails", QString::number(nails_));
    query.replace(":alfalfas", QString::number(alfalfas_));
    query.replace(":eggs", QString::number(eggs_));
    query.replace(":wools", QString::number(wools_));
    query.replace(":upgrade_day", QString::number(upgrade_day_));
    query.replace(":is_upgrading", QString::number(is_upgrading_));
    query.replace(":level", QString::number(level_));

    socket.write(query);
}

void Barn::checkMilksExpiration()
{
    QVector<Milk> milks;

    for (int i = 0; i < milks_.size(); i++)
    {
        if (milks_[i].isExpired())
            milks_[i].remove();
        else
            milks.push_back(milks_[i]);
    }

    milks_ = milks;
}

void Barn::addShovel(int amount)
{
    shovels_ += amount;
    storage_ += amount;
}
void Barn::addNail(int amount)
{
    nails_ += amount;
    storage_ += amount;
}
void Barn::addAlfalfa(int amount)
{
    alfalfas_ += amount;
    storage_ += amount;
}
void Barn::addEgg(int amount)
{
    eggs_ += amount;
    storage_ += amount;
}
void Barn::addWool(int amount)
{
    wools_ += amount;
    storage_ += amount;
}

void Barn::addMilk(const Milk& milk)
{
    milks_.push_back(milk);
    storage_ += 1;
}

void Barn::removeShovel(int amount)
{
    shovels_ -= amount;
    storage_ -= amount;
}
void Barn::removeNail(int amount)
{
    nails_ -= amount;
    storage_ -= amount;
}
void Barn::removeAlfalfa(int amount)
{
    alfalfas_ -= amount;
    storage_ -= amount;
}
void Barn::removeEgg(int amount)
{
    eggs_ -= amount;
    storage_ -= amount;
}
void Barn::removeWool(int amount)
{
    wools_ -= amount;
    storage_ -= amount;
}

void Barn::removeMilk(int milk_id)
{
    for (int i = 0; i < milks_.size(); i++)
    {
        if (milks_[i].id() == milk_id)
        {
            milks_[i].remove();
            milks_.remove(i);
            break;
        }
    }
    storage_ -= 1;
}

void Barn::removeMilk(const Milk& milk)
{
    removeMilk(milk.id());
}

void Milk::save()
{
    QString query = "INSERT INTO Milks(manufacture_day, expiration_day, barn_id)"
                    " VALUES(:manufacture_day, :expiration_day, :barn_id)";
    query.replace(":manufacture_day", QString::number(manufacture_day_));
    query.replace(":expiration_day", QString::number(expiration_day_));
    query.replace(":barn_id", QString::number(barn_id_));

    socket.write(query);
    id_ = socket.read().toInt();
}

void Milk::remove()
{
    QString query = "DELETE FROM Milks WHERE id=:id";
    query.replace(":id", QString::number(id_));
    socket.write(query);
}

bool Milk::isExpired()
{
    return expiration_day_ > CURRENT_DAY;
}

Milk::Milk(int barn_id)
{
    manufacture_day_ = CURRENT_DAY;
    expiration_day_ = CURRENT_DAY + 10;
    barn_id_ = barn_id;
}