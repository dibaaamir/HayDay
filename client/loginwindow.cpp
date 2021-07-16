#include "loginwindow.h"
#include "ui_loginwindow.h"
#include "auth.h"
#include "fields.h"
#include "barn.h"
#include "farm.h"
#include "farmer.h"
#include "messagedialog.h"
#include "mainwindow.h"
#include <QDateTime>

LoginWindow::LoginWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginWindow)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Widget | Qt::MSWindowsFixedSizeDialogHint);

    ui->gboxSignup->setVisible(false);
}

LoginWindow::~LoginWindow()
{
    delete ui;
}

void LoginWindow::on_lblSignup_clicked()
{
    showSignup();
}

void LoginWindow::on_lblLogin_clicked()
{
    showLogin();
}

void LoginWindow::on_btnSignup_clicked()
{
    const QString& username = ui->txtSignupUsername->text();
    const QString& password = ui->txtSignupPassword->text();
    const QString& nickname = ui->txtNickName->text();
    const int gender = ui->cmbGender->currentIndex();
    if(username.isEmpty() || password.isEmpty() || nickname.isEmpty())
    {
        MessageDialog w("Please fill the form.", "Error", this);
        w.exec();
        return;
    }
    if(!isPasswordStrong(password))
    {
        MessageDialog w("Password isn't strong enough.", "Error", this);
        w.exec();
        return;
    }

    Auth auth;
    int account_id = auth.signup(username, password);
    if(account_id)
    {
        AlfalfaField& alfalfa_field = AlfalfaField::create();
        WheatField& wheat_field = WheatField::create();
        Barn& barn = Barn::create();
        Silo& silo = Silo::create();
        Farm& farm = Farm::create(barn.id(), silo.id(), alfalfa_field.id(), wheat_field.id());

        ChickenCoop::create(farm.id());
        CowPasture::create(farm.id());
        SheepPasture::create(farm.id());

        Farmer::create(nickname, gender, account_id, farm.id());

        MessageDialog w("Account has been created successfully.", "Info", this);
        w.exec();

        showLogin();
        ui->txtLoginUsername->setText(username);
    }
    else
    {
        MessageDialog w("Username already exists.", "Error", this);
        w.exec();
    }
}

void LoginWindow::on_btnLogin_clicked()
{
    const QString& username = ui->txtLoginUsername->text();
    const QString& password = ui->txtLoginPassword->text();
    if(username.isEmpty() || password.isEmpty())
    {
        MessageDialog w("Please enter username and password.", "Error", this);
        w.exec();
        return;
    }

    Auth auth;
    int account_id = auth.login(username, password);
    if(account_id)
    {
        Farmer& farmer = Farmer::getByAccountId(account_id);
        Farm& farm = Farm::get(farmer.farm_id());

        CURRENT_DAY = (QDateTime::currentDateTime().toTime_t() - farmer.joining_date()) / SECONDS_PER_DAY;

        MainWindow* main_window = new MainWindow(farmer, farm);
        main_window->show();
        this->close();
    }
    else
    {
        MessageDialog w("Username or password is incorrect.", "Error", this);
        w.exec();
    }
}

bool LoginWindow::isPasswordStrong(const QString &password)
{
    return password.size() >= 4;
}

void LoginWindow::showSignup()
{
    ui->txtLoginUsername->clear();
    ui->txtLoginPassword->clear();

    ui->gboxLogin->setVisible(false);
    ui->gboxSignup->setVisible(true);
    this->setWindowTitle("HayDay - Signup");
}

void LoginWindow::showLogin()
{
    ui->txtSignupUsername->clear();
    ui->txtSignupPassword->clear();
    ui->txtNickName->clear();
    ui->cmbGender->setCurrentIndex(0);

    ui->gboxSignup->setVisible(false);
    ui->gboxLogin->setVisible(true);
    this->setWindowTitle("HayDay - Login");
}
