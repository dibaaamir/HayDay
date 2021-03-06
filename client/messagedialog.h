#ifndef MESSAGEDIALOG_H
#define MESSAGEDIALOG_H

#include <QDialog>
#include <QSound>

namespace Ui {
class MessageDialog;
}

class MessageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MessageDialog(QString text, QString title = "Error", QWidget *parent = nullptr, bool get_num = false);
    ~MessageDialog();

signals:
    void entered_number(int);

private slots:
    void on_btnOk_clicked();

private:
    Ui::MessageDialog *ui;
    QSound* errorSound;
};

#endif // MESSAGEDIALOG_H
