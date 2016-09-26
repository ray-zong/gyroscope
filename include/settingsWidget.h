#pragma once

#include <QWidget>
#include <QSerialPort>

namespace Ui {
class SettingsWidget;
}

class QIntValidator;

QT_END_NAMESPACE

class SettingsWidget : public QWidget
{
    Q_OBJECT

public:
    struct Settings {
        QString name;
        qint32 baudRate;
        QString stringBaudRate;
        QSerialPort::DataBits dataBits;
        QString stringDataBits;
        QSerialPort::Parity parity;
        QString stringParity;
        QSerialPort::StopBits stopBits;
        QString stringStopBits;
        QSerialPort::FlowControl flowControl;
        QString stringFlowControl;
        bool localEchoEnabled;
    };

    explicit SettingsWidget(QWidget *parent = 0);
    ~SettingsWidget();

    Settings settings();

private slots:
    void showPortInfo(int idx);
    void checkCustomBaudRatePolicy(int idx);
    void checkCustomDevicePathPolicy(int idx);

    void emitOpenSerialPort(bool);
    void emitCloseSerialPort(bool);

signals:
    void openSerialPort();
    void closeSerialPort();

private:
    void fillPortsParameters();
    void fillPortsInfo();
    void updateSettings();

private:
    Ui::SettingsWidget *ui;
    Settings currentSettings;
    QIntValidator *intValidator;
};
