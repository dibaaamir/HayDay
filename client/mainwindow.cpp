#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "detailsdialog.h"
#include "barndetailsdialog.h"
#include "shopdialog.h"
#include "levelupdialog.h"
#include "messagedialog.h"
#include <QThread>
#include "scoreboarddialog.h"

MainWindow::MainWindow(Farmer& farmer, Farm& farm, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), farmer(farmer), farm(farm)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window | Qt::MSWindowsFixedSizeDialogHint);

    initUI();
    checkTimeRelatedFunctions(false);

    //Connect Timer
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::anotherDayPassed);
    timer->start(static_cast<int>(SECONDS_PER_DAY) * 1000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::anotherDayPassed()
{
    CURRENT_DAY += 1;

    checkTimeRelatedFunctions(true);

    showDay();
}

void MainWindow::initUI()
{
    showCoin();
    showDay();
    showLevel();
    showXP();
    showAnimals();

    if(farm.alfalfa_field().level())
        unlockAlfalfaField();

    if(farm.wheat_field().plants_condition() == Enums::PLANTED)
        on_wheatFieldPlanted();
    if(farm.alfalfa_field().plants_condition() == Enums::PLANTED)
        on_alfalfaFieldPlanted();

    if(farmer.level() >= 2)
        unlockShop();
}

void MainWindow::checkTimeRelatedFunctions(bool add_day_xp)
{
    int xp_to_add = add_day_xp;

    if(farm.silo().is_upgrading() && farm.silo().isUpgradeFinished())
    {
        farm.silo().finishUpgrade();
        xp_to_add += farm.silo().upgradeXp();
        MessageDialog w("Silo is now level " + QString::number(farm.silo().level()), "Info", this);
        w.exec();
    }
    if(farm.barn().is_upgrading() && farm.barn().isUpgradeFinished())
    {
        farm.barn().finishUpgrade();
        xp_to_add += farm.barn().upgradeXp();
        MessageDialog w("Barn is now level " + QString::number(farm.barn().level()), "Info", this);
        w.exec();
    }
    if(farm.chicken_coop().is_upgrading() && farm.chicken_coop().isUpgradeFinished())
    {
        if(farm.chicken_coop().level() == 0)
            showAnimals();
        farm.chicken_coop().finishUpgrade();
        xp_to_add += farm.chicken_coop().upgradeXp();
        MessageDialog w("Chicken Coop is now level " + QString::number(farm.chicken_coop().level()), "Info", this);
        w.exec();
    }
    if(farm.cow_pasture().is_upgrading() && farm.cow_pasture().isUpgradeFinished())
    {
        if(farm.cow_pasture().level() == 0)
            showAnimals();
        farm.cow_pasture().finishUpgrade();
        xp_to_add += farm.cow_pasture().upgradeXp();
        MessageDialog w("Cow Pasture is now level " + QString::number(farm.cow_pasture().level()), "Info", this);
        w.exec();
    }
    if(farm.sheep_pasture().is_upgrading() && farm.sheep_pasture().isUpgradeFinished())
    {
        if(farm.sheep_pasture().level() == 0)
            showAnimals();
        farm.sheep_pasture().finishUpgrade();
        xp_to_add += farm.sheep_pasture().upgradeXp();
        MessageDialog w("Sheep Pasture is now level " + QString::number(farm.sheep_pasture().level()), "Info", this);
        w.exec();
    }
    if(farm.alfalfa_field().is_upgrading() && farm.alfalfa_field().isUpgradeFinished())
    {
        farm.alfalfa_field().finishUpgrade();
        xp_to_add += farm.alfalfa_field().upgradeXp();
        unlockAlfalfaField();
        MessageDialog w("Alfalfa Field is now level " + QString::number(farm.alfalfa_field().level()), "Info", this);
        w.exec();
    }
    if(farm.wheat_field().is_upgrading() && farm.wheat_field().isUpgradeFinished())
    {
        farm.wheat_field().finishUpgrade();
        xp_to_add += farm.wheat_field().upgradeXp();
        MessageDialog w("Wheat Field is now level " + QString::number(farm.wheat_field().level()), "Info", this);
        w.exec();
    }
    if(farm.alfalfa_field().plants_condition() == Enums::PLOWING && farm.alfalfa_field().isPlowingFinished())
        farm.alfalfa_field().finishPlowing();

    on_xp_add(xp_to_add);
}

void MainWindow::showAnimals()
{
    if(farm.chicken_coop().level() > 0)
        unlockChickenCoop();
    if(farm.cow_pasture().level() > 0)
        unlockCowPasture();
    if(farm.sheep_pasture().level() > 0)
        unlockSheepPasture();

    showChickens(farm.chicken_coop().storage());
    showCows(farm.cow_pasture().storage());
    showSheeps(farm.cow_pasture().storage());
}

void MainWindow::unlockAlfalfaField()
{
    if(ui->alfalfaLock)
    {
        ui->alfalfaLock->deleteLater();
        ui->alfalfaLock = nullptr;
        ui->btnAlfalfaField->setEnabled(true);
        ui->btnAlfalfaField->setCursor(Qt::PointingHandCursor);
        //Change picture from dark to light on focus
        ui->btnAlfalfaField->setStyleSheet("QPushButton{\n	border: none;\n	background-image: url(:/img/field-dark.png);\n}\n\nQPushButton:hover{\n	background-image: url(:/img/field.png);\n}");
    }
}

void MainWindow::unlockChickenCoop()
{
    if(ui->chickenLock)
    {
        ui->chickenLock->deleteLater();
        ui->chickenLock = nullptr;
        ui->btnChickenCoop->setEnabled(true);
        ui->btnChickenCoop->setCursor(Qt::PointingHandCursor);
        //Change picture from dark to light on focus
        ui->btnChickenCoop->setStyleSheet("QPushButton{\n	border: none;\n	background-image: url(:/img/chicken_coop-dark.png);\n}\n\nQPushButton:hover{\n	background-image: url(:/img/chicken_coop.png);\n}");
    }
}

void MainWindow::unlockCowPasture()
{
    if(ui->cowLock)
    {
        ui->cowLock->deleteLater();
        ui->cowLock = nullptr;
        ui->btnCowPasture->setEnabled(true);
        ui->btnCowPasture->setCursor(Qt::PointingHandCursor);
        //Change picture from dark to light on focus
        ui->btnSheepPasture->setStyleSheet("QPushButton{\n	border: none;\n	background-image: url(:/img/pasture-dark.png);\n}\n\nQPushButton:hover{\n	background-image: url(:/img/pasture.png);\n}");
    }
}

void MainWindow::unlockSheepPasture()
{
    if(ui->sheepLock)
    {
        ui->sheepLock->deleteLater();
        ui->sheepLock = nullptr;
        ui->btnSheepPasture->setEnabled(true);
        ui->btnSheepPasture->setCursor(Qt::PointingHandCursor);
        //Change picture from dark to light on focus
        ui->btnSheepPasture->setStyleSheet("QPushButton{\n	border: none;\n	background-image: url(:/img/pasture-dark.png);\n}\n\nQPushButton:hover{\n	background-image: url(:/img/pasture.png);\n}");
    }
}

void MainWindow::unlockShop()
{
    if(ui->shopLock)
    {
        ui->shopLock->deleteLater();
        ui->shopLock = nullptr;
        ui->btnShop->setEnabled(true);
        ui->btnShop->setCursor(Qt::PointingHandCursor);
        //Change picture from dark to light on focus
        ui->btnShop->setStyleSheet("QPushButton{\n	border: none;\n	background-image: url(:/img/shop-dark.png);\n}\n\nQPushButton:hover{\n	background-image: url(:/img/shop.png);\n}");
    }
}

void MainWindow::showChickens(int count)
{
    ui->btnChicken_1->setVisible(false);
    ui->btnChicken_2->setVisible(false);
    ui->btnChicken_3->setVisible(false);
    ui->btnChicken_4->setVisible(false);
    ui->btnChicken_5->setVisible(false);
    ui->btnChicken_6->setVisible(false);

    if(count >= 1)
        ui->btnChicken_1->setVisible(true);
    if(count >= 2)
        ui->btnChicken_2->setVisible(true);
    if(count >= 3)
        ui->btnChicken_3->setVisible(true);
    if(count >= 4)
        ui->btnChicken_4->setVisible(true);
    if(count >= 5)
        ui->btnChicken_5->setVisible(true);
    if(count >= 6)
        ui->btnChicken_6->setVisible(true);
}

void MainWindow::showCows(int count)
{
    ui->btnCow_1->setVisible(false);
    ui->btnCow_2->setVisible(false);
    ui->btnCow_3->setVisible(false);
    ui->btnCow_4->setVisible(false);

    if(count >= 1)
        ui->btnCow_1->setVisible(true);
    if(count >= 2)
        ui->btnCow_2->setVisible(true);
    if(count >= 3)
        ui->btnCow_3->setVisible(true);
    if(count >= 4)
        ui->btnCow_4->setVisible(true);
}

void MainWindow::showSheeps(int count)
{
    ui->btnSheep_1->setVisible(false);
    ui->btnSheep_2->setVisible(false);
    ui->btnSheep_3->setVisible(false);
    ui->btnSheep_4->setVisible(false);
    ui->btnSheep_5->setVisible(false);
    ui->btnSheep_6->setVisible(false);

    if(count >= 1)
        ui->btnSheep_1->setVisible(true);
    if(count >= 2)
        ui->btnSheep_2->setVisible(true);
    if(count >= 3)
        ui->btnSheep_3->setVisible(true);
    if(count >= 4)
        ui->btnSheep_4->setVisible(true);
    if(count >= 5)
        ui->btnSheep_5->setVisible(true);
    if(count >= 6)
        ui->btnSheep_6->setVisible(true);
}

void MainWindow::showDay()
{
    ui->lblDay->setNum(static_cast<int>(CURRENT_DAY));
}

void MainWindow::showCoin()
{
    ui->lblCoin->setNum(farmer.coins());
}

void MainWindow::showXP()
{
    ui->lblXP->setText(QString::number(farmer.xp()) + " / " + QString::number(farmer.max_xp()));

    if(farmer.xp())
        ui->pbar_left_corner->setVisible(true);
    else
        ui->pbar_left_corner->setVisible(false);

    if(farmer.xp() > 1)
    {
        const int progress = static_cast<int>((static_cast<float>(farmer.xp()) / farmer.max_xp()) * 100);
        const int width_of_center_pbar = 196;
        ui->pbar_center->resize(width_of_center_pbar * (progress - 1) / 98, 20);
        ui->pbar_center->setVisible(true);
    }
    else
        ui->pbar_center->setVisible(false);
}

void MainWindow::showLevel()
{
    ui->lblLevel->setText(QString::number(farmer.level()));
}

void MainWindow::on_btnSilo_clicked()
{
    DetailsDialog details("Silo", farmer, farm, this);
    details.exec();
    showCoin();
}

void MainWindow::on_btnChickenCoop_clicked()
{
    DetailsDialog details("Chicken Coop", farmer, farm, this);
    connect(&details, &DetailsDialog::AddXP, this, &MainWindow::on_xp_add);
    details.exec();
    showCoin();
    showAnimals();
}

void MainWindow::on_btnCowPasture_clicked()
{
    DetailsDialog details("Cow Pasture", farmer, farm, this);
    connect(&details, &DetailsDialog::AddXP, this, &MainWindow::on_xp_add);
    details.exec();
    showCoin();
    showAnimals();
}

void MainWindow::on_btnSheepPasture_clicked()
{
    DetailsDialog details("Sheep Pasture", farmer, farm, this);
    connect(&details, &DetailsDialog::AddXP, this, &MainWindow::on_xp_add);
    details.exec();
    showCoin();
    showAnimals();
}

void MainWindow::on_chickenLock_clicked()
{
    if(farmer.level() >= 2)
        on_btnChickenCoop_clicked();
    else
    {
        MessageDialog w("You have not reached required level to unlock", "Error", this);
        w.exec();
    }
}

void MainWindow::on_cowLock_clicked()
{
    if(farmer.level() >= 4)
        on_btnCowPasture_clicked();
    else
    {
        MessageDialog w("You have not reached required level to unlock", "Error", this);
        w.exec();
    }
}

void MainWindow::on_sheepLock_clicked()
{
    if(farmer.level() >= 6)
        on_btnSheepPasture_clicked();
    else
    {
        MessageDialog w("You have not reached required level to unlock", "Error", this);
        w.exec();
    }
}

void MainWindow::on_btnBarn_clicked()
{
    BarnDetailsDialog details(farmer, farm, this);
    details.exec();
    showCoin();
}

void MainWindow::on_btnWheatField_clicked()
{
    DetailsDialog details("Wheat Field", farmer, farm, this);
    connect(&details, &DetailsDialog::WheatFieldPlanted, this, &MainWindow::on_wheatFieldPlanted);
    connect(&details, &DetailsDialog::WheatFieldReaped, this, &MainWindow::on_wheatFieldReaped);
    connect(&details, &DetailsDialog::AddXP, this, &MainWindow::on_xp_add);
    details.exec();
    showCoin();
}

void MainWindow::on_btnAlfalfaField_clicked()
{
    int level = farm.alfalfa_field().level();
    DetailsDialog details("Alfalfa Field", farmer, farm, this);
    connect(&details, &DetailsDialog::AlfalfaFieldPlanted, this, &MainWindow::on_alfalfaFieldPlanted);
    connect(&details, &DetailsDialog::AlfalfaFieldReaped, this, &MainWindow::on_alfalfaFieldReaped);
    connect(&details, &DetailsDialog::AddXP, this, &MainWindow::on_xp_add);
    details.exec();
    showCoin();
    if(level == 0 && farm.alfalfa_field().level())
        unlockAlfalfaField();
}

void MainWindow::on_alfalfaLock_clicked()
{
    if(farmer.level() >= 3)
        on_btnAlfalfaField_clicked();
    else
    {
        MessageDialog w("You have not reached required level to unlock", "Error", this);
        w.exec();
    }
}

void MainWindow::on_shopLock_clicked()
{
    if(farmer.level() >= 2)
        on_btnShop_clicked();
    else
    {
        MessageDialog w("You have not reached required level to unlock", "Error", this);
        w.exec();
    }
}

void MainWindow::on_btnShop_clicked()
{
    ShopDialog shop(farmer, this);
    connect(&shop, &ShopDialog::AddXP, this, &MainWindow::on_xp_add);
    shop.exec();
    showCoin();
    showAnimals();
}

void MainWindow::on_btnScoreBoard_clicked()
{
    ScoreboardDialog board(this);
    board.exec();
}

void MainWindow::on_wheatFieldPlanted()
{
    ui->btnWheatField->setStyleSheet("QPushButton{\n	border: none;\n	background-image: url(:/img/wheat-field-dark.png);\n}\n\nQPushButton:hover{\n	background-image: url(:/img/wheat-field.png);\n}");
}

void MainWindow::on_wheatFieldReaped()
{
    ui->btnWheatField->setStyleSheet("QPushButton{\n	border: none;\n	background-image: url(:/img/field-dark.png);\n}\n\nQPushButton:hover{\n	background-image: url(:/img/field.png);\n}");
}

void MainWindow::on_alfalfaFieldPlanted()
{
    ui->btnAlfalfaField->setStyleSheet("QPushButton{\n	border: none;\n	background-image: url(:/img/alfalfa-field-dark.png);\n}\n\nQPushButton:hover{\n	background-image: url(:/img/alfalfa-field.png);\n}");
}

void MainWindow::on_alfalfaFieldReaped()
{
    ui->btnAlfalfaField->setStyleSheet("QPushButton{\n	border: none;\n	background-image: url(:/img/field-dark.png);\n}\n\nQPushButton:hover{\n	background-image: url(:/img/field.png);\n}");
}

void MainWindow::on_xp_add(int amount)
{
    if(farmer.addXp(amount))
    {
        showLevel();
        LevelUpDialog dialog(farmer.level(), this);
        dialog.exec();
        if(farmer.level() == 2)
            unlockShop();
    }
    showXP();
}
