#include "dialog.h"
#include "ui_dialog.h"
#include <QtWidgets>

namespace json
{
    QVariantMap toMap(const QByteArray &val)
    {
        QJsonParseError jError;
        QJsonDocument jDoc = QJsonDocument::fromJson(val, &jError);
        if (jError.error == QJsonParseError::NoError) {
            if (jDoc.isObject()) {
                QJsonObject jObj = jDoc.object();
                return jObj.toVariantMap();
            }
        }
        QVariantMap ret;
        return ret;
    }

    QString toString(const QVariantMap &val)
    {
        QJsonObject jobj = QJsonObject::fromVariantMap(val);
        QJsonDocument jdoc(jobj);
        return QString(jdoc.toJson(QJsonDocument::Compact));
    }
}

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags() | Qt::WindowMinMaxButtonsHint);
    connect(ui->pbStart, SIGNAL(clicked()), this, SLOT(slotStart()));
    connect(ui->pbLoad, SIGNAL(clicked()), this, SLOT(slotLoad()));
    connect(ui->pbClear, SIGNAL(clicked()), this, SLOT(slotClear()));
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::slotStart()
{
    QString address = ui->leAddress->text().trimmed();
    QFile file(address);
    QStringList strList;
    if (file.exists()) {
        file.open(QIODevice::ReadOnly);
        QVariantMap data = json::toMap(file.readAll());
        foreach (const QVariant &item, data["configs"].toList()) {
            QString str = item.toMap()["server"].toString();
            if (!str.isEmpty()) {
                strList << str;
            }
        }
    } else if (!address.isEmpty()) {
        strList << address;
    }

    foreach (QString ip, strList) {
        QProcess *process = new QProcess();
        connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(slotReadStandardOutput()));
        connect(process, SIGNAL(readyReadStandardError()), this, SLOT(slotReadStandardError()));
        connect(process, SIGNAL(finished(int)), this, SLOT(slotFinished(int)));
        QDir dir = QDir::root();
        dir.cd("Windows\\System32");
        process->start(dir.path() + "\\ping.exe", QStringList() << ip);
    }
}

void Dialog::slotLoad()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    ui->leAddress->setText(fileName);
}

void Dialog::slotClear()
{
    ui->lwResult->clear();
}

void Dialog::slotReadStandardOutput()
{
    QProcess *process = dynamic_cast<QProcess*>(sender());
    char buffer[1024] = {0};
    process->readLine(buffer, 1024);

    QString strOutput = QString::fromLocal8Bit(process->readAllStandardOutput());
    if (!strOutput.isEmpty()) {
        ui->lwResult->addItem(strOutput.replace("\n", ""));
        ui->lwResult->scrollToBottom();
    }
}

void Dialog::slotReadStandardError()
{
}

void Dialog::slotFinished(int)
{
    QProcess *process = dynamic_cast<QProcess*>(sender());
    process->deleteLater();
}

