#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

namespace Ui {
class Dialog;
}

class QProcess;
class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

protected slots:
    void slotStart();
    void slotLoad();
    void slotClear();
    void slotReadStandardOutput();
    void slotReadStandardError();
    void slotFinished(int);

private:
    Ui::Dialog *ui;
};

#endif // DIALOG_H
