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

    enum SwitchFlag: unsigned char
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
    void analysisData_X(const QByteArray &data);
    void analysisData_Y(const QByteArray &data);
    //发送控制信息
    void controlInfo();

    inline void changeSwitchState(unsigned char &szSwitch, int num, int state);

    //初始化串口//
    void initSerialPort();
    //初始化定时器//
    void initTimer();

private slots:
    //不作串口区分
    void handleError(QSerialPort::SerialPortError error);
    void closeSerialPort();

    //区分不同串口
    void openSerialPort_gyroscope();
    void readData_gyroscope();

    void openSerialPort_control();
    void readData_control();

    void openSerialPort_X();
    void readData_X();

    void openSerialPort_Y();
    void readData_Y();

    //发送激光控制指令
    void sendControlCommand();

    void timeout_delayTimer1();
    void timeout_delayTimer2();
    void timeout_delayTimer3();

    //启动延迟定时器1//
    void startDelayTime1(int ms);
    //启动延迟定时器2//
    void startDelayTime2(int ms);
    //启动延迟定时器3//
    void startDelayTime3(int ms);

private:
    Ui::MainWindow *ui;

    //陀螺仪//
    QSerialPort *m_pSerialPort_gyroscope;
    SettingsWidget *m_pSettingsWidget_gyroscope;
    bool m_bUpdatedAngle;

    //控制器//
    QSerialPort *m_pSerialPort_control;
    SettingsWidget *m_pSettingsWidget_control;

    //激光阵列:X//
    QSerialPort *m_pSerialPort_X;
    SettingsWidget *m_pSettingsWidget_X;
    int m_flagArrayX[12];
    bool m_bUpdatedX;

    //激光阵列:Y//
    QSerialPort *m_pSerialPort_Y;
    SettingsWidget *m_pSettingsWidget_Y;
    int m_flagArrayY[12];
    bool m_bUpdatedY;

    QLabel *m_pLabelStatus;
    QTimer *m_pTimer_delay1; //第一次延时控制//
    QTimer *m_pTimer_delay2; //第二次延时控制//
    QTimer *m_pTimer_delay3; //第三次延时控制//
};

#endif // MAINWINDOW_H
