#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class SettingsWidget;
class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    enum switchFlag: unsigned char
    {
        ///标志位表示仅有一路开关打开，其他关闭
        ///例如：需要打开1路控制阀，则 &= flag1
        ///     需要关闭1路控制阀，则 |= (~flag1)
        flag1 = 0x7F, //0111 1111
        flag2 = 0xBF, //1011 1111
        flag3 = 0xDF, //1101 1111
        flag4 = 0xEF, //1110 1111
        flag5 = 0xF7, //1111 0111
        flag6 = 0xFB, //1111 1011
    };


private:
    //状态栏显示消息
    void showStatusMessage(const QString &message);

    //解析数据
    void analysisData_1(const QByteArray &data);
    void analysisData_2(const QByteArray &data);
    void analysisData_X(const QByteArray &data);
    void analysisData_Y(const QByteArray &data);
    //发送控制信息
    void controlInfo();

    inline void changeSwitchState(unsigned char &szSwitch, int num, int state);

	//double calCommand(double x, double y, );

private slots:
    //不作串口区分
    void handleError(QSerialPort::SerialPortError error);
    void closeSerialPort();

    //区分不同串口
    void openSerialPort_1();
    void readData_1();

    void openSerialPort_2();
    void readData_2();

    void openSerialPort_X();
    void readData_X();

    void openSerialPort_Y();
    void readData_Y();

    //发送激光控制指令
    void sendControlCommand();

    //启动定时器//
    void startSendCommand();

    //停止定时器//
    void stopSendCommand();

    void timeout();


private:
    Ui::MainWindow *ui;

    //串口1:陀螺仪
    QSerialPort *m_pSerialPort_1;
    SettingsWidget *m_pSettingsWidget_1;
    bool m_bUpdatedAngle;

    //串口2:控制器
    QSerialPort *m_pSerialPort_2;
    SettingsWidget *m_pSettingsWidget_2;

    //激光阵列:X
    QSerialPort *m_pSerialPort_X;
    SettingsWidget *m_pSettingsWidget_X;
    int m_flagArrayX[12];
    bool m_bUpdatedX;

    //激光阵列:Y
    QSerialPort *m_pSerialPort_Y;
    SettingsWidget *m_pSettingsWidget_Y;
    int m_flagArrayY[12];
    bool m_bUpdatedY;

    QLabel *m_pLabelStatus;
    QTimer *m_pTimer;
};

#endif // MAINWINDOW_H
