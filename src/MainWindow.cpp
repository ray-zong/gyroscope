#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <cstdint>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QDebug>
#include <QLabel>

#include "settingsWidget.h"

MainWindow::MainWindow(QWidget *parent) :
QMainWindow(parent),
	ui(new Ui::MainWindow),
    m_pSerialPort_gyroscope(NULL),
    m_pSettingsWidget_gyroscope(NULL),
    m_pSerialPort_control(NULL),
    m_pSettingsWidget_control(NULL),
	m_pLabelStatus(NULL),
	m_bUpdatedX(false),
	m_bUpdatedY(false),
    m_bUpdatedAngle(false),
    m_pTimer_delay1(NULL),
    m_pTimer_delay2(NULL),
    m_pTimer_delay3(NULL)
{
	ui->setupUi(this);

    //信息提示//
	m_pLabelStatus = new QLabel;
	ui->statusBar->addWidget(m_pLabelStatus);

    //初始化串口//
    initSerialPort();
    //初始化定时器//
    initTimer();

    connect(ui->pushButton_control, SIGNAL(clicked(bool)), this, SLOT(sendControlCommand()));
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::handleError(QSerialPort::SerialPortError error)
{
	QSerialPort *pSerialPort = dynamic_cast<QSerialPort *>(sender());
	if(pSerialPort == NULL)
		return;

	if (error == QSerialPort::ResourceError) {
		QMessageBox::critical(this, tr("Critical Error"), pSerialPort->errorString());
		closeSerialPort();
	}
}

void MainWindow::closeSerialPort()
{
	QSerialPort *pSerialPort = dynamic_cast<QSerialPort *>(sender());
	if(pSerialPort == NULL)
		return;

	if (pSerialPort->isOpen())
		pSerialPort->close();

	showStatusMessage(tr("Disconnected"));
}

void MainWindow::openSerialPort_gyroscope()
{
    SettingsWidget::Settings p = m_pSettingsWidget_gyroscope->settings();
    m_pSerialPort_gyroscope->setPortName(p.name);
    m_pSerialPort_gyroscope->setBaudRate(p.baudRate);
    m_pSerialPort_gyroscope->setDataBits(p.dataBits);
    m_pSerialPort_gyroscope->setParity(p.parity);
    m_pSerialPort_gyroscope->setStopBits(p.stopBits);
    m_pSerialPort_gyroscope->setFlowControl(p.flowControl);
    if (m_pSerialPort_gyroscope->open(QIODevice::ReadWrite)) {
		showStatusMessage(tr("Connected to %1 : %2, %3, %4, %5, %6")
			.arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
			.arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl));
	} else {
        QMessageBox::critical(this, tr("Error"), m_pSerialPort_gyroscope->errorString());
		showStatusMessage(tr("Open error"));
	}
}

void MainWindow::readData_gyroscope()
{
    static QByteArray byteArray;
    byteArray += m_pSerialPort_gyroscope->readAll();

    static const char szBeginFlag[4] = {0x88, 0xAF, 0x1C, 0x00};
    int index = byteArray.indexOf(szBeginFlag, 0);

    if(index != -1 && byteArray.size() >= 32 + index)
	{
        //提取29个
        unsigned char szData[32] = {};
        memcpy(szData, byteArray.mid(index, 32).data(), sizeof(szData));

        //检验//
        //提取//

        //角速度//
        int16_t gyroscope = 0;
		memcpy(&gyroscope, szData + 13, sizeof(int16_t));
		gyroscope = ((gyroscope & 0x00FF) << 8) | ((gyroscope & 0xFF00) >> 8);

        //角度//
        int16_t temperature = 0;
		memcpy(&temperature, szData + 25, sizeof(int16_t));
		temperature = ((temperature & 0x00FF) << 8) | ((temperature & 0xFF00) >> 8);

        ui->doubleSpinBox_1->setValue(gyroscope);

		ui->doubleSpinBox_2->setValue(temperature);
		m_bUpdatedAngle = true;

        //清除数据//
        byteArray.clear();
	}

    if(byteArray.size() > 1000)
    {
        byteArray.clear();
    }
}

void MainWindow::openSerialPort_control()
{
    SettingsWidget::Settings p = m_pSettingsWidget_control->settings();
    m_pSerialPort_control->setPortName(p.name);
    m_pSerialPort_control->setBaudRate(p.baudRate);
    m_pSerialPort_control->setDataBits(p.dataBits);
    m_pSerialPort_control->setParity(p.parity);
    m_pSerialPort_control->setStopBits(p.stopBits);
    m_pSerialPort_control->setFlowControl(p.flowControl);
    if (m_pSerialPort_control->open(QIODevice::ReadWrite)) {
		showStatusMessage(tr("Connected to %1 : %2, %3, %4, %5, %6")
			.arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
			.arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl));
	} else {
        QMessageBox::critical(this, tr("Error"), m_pSerialPort_control->errorString());

		showStatusMessage(tr("Open error"));
	}
}

void MainWindow::readData_control()
{
    QByteArray data = m_pSerialPort_control->readAll();
    qDebug() << __FILE__ << __LINE__ << "\trecv:\t" << data;
}

void MainWindow::openSerialPort_X()
{
	SettingsWidget::Settings p = m_pSettingsWidget_X->settings();
	m_pSerialPort_X->setPortName(p.name);
	m_pSerialPort_X->setBaudRate(p.baudRate);
	m_pSerialPort_X->setDataBits(p.dataBits);
	m_pSerialPort_X->setParity(p.parity);
	m_pSerialPort_X->setStopBits(p.stopBits);
	m_pSerialPort_X->setFlowControl(p.flowControl);
	if (m_pSerialPort_X->open(QIODevice::ReadWrite)) {
		showStatusMessage(tr("Connected to %1 : %2, %3, %4, %5, %6")
			.arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
			.arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl));
	} else {
		QMessageBox::critical(this, tr("Error"), m_pSerialPort_X->errorString());

		showStatusMessage(tr("Open error"));
	}
}

void MainWindow::readData_X()
{
	QByteArray data = m_pSerialPort_X->readAll();
	analysisData_X(data);
}

void MainWindow::openSerialPort_Y()
{
	SettingsWidget::Settings p = m_pSettingsWidget_Y->settings();
	m_pSerialPort_Y->setPortName(p.name);
	m_pSerialPort_Y->setBaudRate(p.baudRate);
	m_pSerialPort_Y->setDataBits(p.dataBits);
	m_pSerialPort_Y->setParity(p.parity);
	m_pSerialPort_Y->setStopBits(p.stopBits);
	m_pSerialPort_Y->setFlowControl(p.flowControl);
	if (m_pSerialPort_Y->open(QIODevice::ReadWrite)) {
		showStatusMessage(tr("Connected to %1 : %2, %3, %4, %5, %6")
			.arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
			.arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl));
	} else {
		QMessageBox::critical(this, tr("Error"), m_pSerialPort_Y->errorString());

		showStatusMessage(tr("Open error"));
	}
}

void MainWindow::readData_Y()
{
	QByteArray data = m_pSerialPort_Y->readAll();
	analysisData_Y(data);
}

void MainWindow::sendControlCommand()
{
	const static QByteArray c_szCommand(1, 0xC0);
	if(m_pSerialPort_X->isOpen())
	{
		m_pSerialPort_X->write(c_szCommand);
	}

	if(m_pSerialPort_Y->isOpen())
	{
		m_pSerialPort_Y->write(c_szCommand);
	}
}

void MainWindow::timeout_delayTimer1()
{
    if(m_pTimer_delay1 != NULL)
    {
        m_pTimer_delay1->stop();
    }
    //发送命令//

    //计算延时//
    int ms = 0;
    startDelayTime2(ms);
}

void MainWindow::timeout_delayTimer2()
{
    if(m_pTimer_delay2 != NULL)
    {
        m_pTimer_delay2->stop();
    }
    //发送命令//

    //计算延时//
    int ms = 0;
    startDelayTime3(ms);
}

void MainWindow::timeout_delayTimer3()
{
    if(m_pTimer_delay3 != NULL)
    {
        m_pTimer_delay3->stop();
    }
    //发送命令//
    if(ui->radioButton->isChecked())
    {
        sendControlCommand();
    }
}

void MainWindow::startDelayTime1(int ms)
{
    if(m_pTimer_delay1 != NULL)
    {
        m_pTimer_delay1->start(ms);
    }
}

void MainWindow::startDelayTime2(int ms)
{
    if(m_pTimer_delay2 != NULL)
    {
        m_pTimer_delay2->start(ms);
    }
}

void MainWindow::startDelayTime3(int ms)
{
    if(m_pTimer_delay3 != NULL)
    {
        m_pTimer_delay3->start(ms);
    }
}

void MainWindow::showStatusMessage(const QString &message)
{
	m_pLabelStatus->setText(message);
}

void MainWindow::analysisData_X(const QByteArray &data)
{
	memset(m_flagArrayX, 0, 12);

	//开始标识位
	static QByteArray byteArray;
	byteArray += data;
	static const unsigned char c_szBeginFlag[3] = {0xCD, 0xCE, 0x00};

	int index = data.indexOf((char*)c_szBeginFlag, 0);
	while(index != -1 && byteArray.size() > 3)
	{
		//提取4个字节
		unsigned char szData[4] = {};
		memcpy(szData, byteArray.mid(index, 4).data(), sizeof(szData));
		{
			m_flagArrayX[0] = szData[2] & 1;
			m_flagArrayX[1] = szData[2] >> 1 & 1;
			m_flagArrayX[2] = szData[2] >> 2 & 1;
			m_flagArrayX[3] = szData[2] >> 3 & 1;
			m_flagArrayX[4] = szData[2] >> 4 & 1;
			m_flagArrayX[5] = szData[2] >> 5 & 1;
			m_flagArrayX[6] = szData[3] & 1;
			m_flagArrayX[7] = szData[3] >> 1 & 1;
			m_flagArrayX[8] = szData[3] >> 2 & 1;
			m_flagArrayX[9] = szData[3] >> 3 & 1;
			m_flagArrayX[10] = szData[3] >> 4 & 1;
			m_flagArrayX[11] = szData[3] >> 5 & 1;
			m_bUpdatedX = true;
		}
		//移除开始字节前+4字节的数据
		byteArray.remove(0, index + 4);
		//重新搜索开始标识位
		index = byteArray.indexOf((char*)c_szBeginFlag, 0);
	}
	controlInfo();
}

void MainWindow::analysisData_Y(const QByteArray &data)
{
	memset(m_flagArrayY, 0, 12);
	//开始标识位
	static QByteArray byteArray;
	byteArray += data;
	static const unsigned char c_szBeginFlag[3] = {0xCD, 0xCE, 0x00};

	int index = data.indexOf((char*)c_szBeginFlag, 0);
	while(index != -1 && byteArray.size() > 3)
	{
		//提取4个字节
		unsigned char szData[4] = {};
		memcpy(szData, byteArray.mid(index, 4).data(), sizeof(szData));
		{
			m_flagArrayY[0] = szData[2] & 1;
			m_flagArrayY[1] = szData[2] >> 1 & 1;
			m_flagArrayY[2] = szData[2] >> 2 & 1;
			m_flagArrayY[3] = szData[2] >> 3 & 1;
			m_flagArrayY[4] = szData[2] >> 4 & 1;
			m_flagArrayY[5] = szData[2] >> 5 & 1;
			m_flagArrayY[6] = szData[3] & 1;
			m_flagArrayY[7] = szData[3] >> 1 & 1;
			m_flagArrayY[8] = szData[3] >> 2 & 1;
			m_flagArrayY[9] = szData[3] >> 3 & 1;
			m_flagArrayY[10] = szData[3] >> 4 & 1;
			m_flagArrayY[11] = szData[3] >> 5 & 1;
			m_bUpdatedY = true;
		}
		//移除开始字节前+4字节的数据
		byteArray.remove(0, index + 4);
		//重新搜索开始标识位
		index = byteArray.indexOf((char*)c_szBeginFlag, 0);
	}
	controlInfo();
}

void MainWindow::controlInfo()
{
	if(m_bUpdatedX && m_bUpdatedY/* && m_bUpdatedAngle*/)
	{
		//求X, Y坐标
		double x = 0;
		double y = 0;
		int count_x = 0;
		int count_y = 0;
		for(int i = 0; i < 12; ++i)
		{
			if(m_flagArrayX[i] == 0)
			{
				x += i;
				++count_x;
			}

			if(m_flagArrayY[i] == 0)
			{
				y += i;
				++count_y;
			}
		}	

		if(count_x == 0 || count_y == 0)
		{
			return ;
		}
		x /= count_x;
		y /= count_y;
		
		ui->doubleSpinBox_x->setValue(x);
		ui->doubleSpinBox_y->setValue(y);

		//起始标志位
		static const unsigned char c_szBeginFlag = 0x24;
		//终止标志位
		static const unsigned char c_szEndFlag = 0x0D;
		//1-6号电磁阀
		unsigned char sz1_6 = 0xFF;
		//7-12号电磁阀
		unsigned char sz7_12 = 0xFF;
		//13-18号电磁阀
		unsigned char sz13_18 = 0xFF;
		//19-24号电磁阀
		unsigned char sz19_24 = 0xFF;

		//调用函数设置控制阀的开关
		//changeSwitchState(sz1_6, 1, 0);
		//changeSwitchState(sz13_18, 13, 0);
        //ui->spinBox->setValue(time);
		//控制算法
		double omega = 0.061*ui->doubleSpinBox_1->value();
		double angle = 0.1*ui->doubleSpinBox_2->value();
		double k=1;//可调
		double l=1;//可调
		double m=1;//可调
		double t1=0;
		double t2=0;
		double t3=0;
		int Xpush[8]={0,0,0,0,0,0,0,0};
		int Ypush[8]={0,0,0,0,0,0,0,0};
		int Zpush[8]={0,0,0,0,0,0,0,0};
		if (x>7){Xpush[5]=1;Xpush[6]=1;t1=k*100*(x-5.5);}
		if (x<4){Xpush[1]=1;Xpush[2]=1;t1=k*100*(5.5-x);}
		if (y>7){Ypush[7]=1;Ypush[8]=1;t2=k*100*(y-5.5);}
		if (y<4){Ypush[3]=1;Ypush[4]=1;t2=k*100*(5.5-y);}		
		t3=l*omega+0.5*l*l*angle;
		if(abs(t3)<m) t3=0;
		else if(t3<0){Zpush[2]=1,Zpush[4]=1;Zpush[6]=1,Zpush[8]=1;t3=-t3;}
		else {Zpush[1]=1;Zpush[3]=1;Zpush[5]=1;Zpush[7]=1;}

        //算法结束

		//组装数据
		QByteArray szData;
		szData.push_back(c_szBeginFlag);
		szData.push_back(sz1_6);
		szData.push_back(sz7_12);
		szData.push_back(sz13_18);
		szData.push_back(sz19_24);
		szData.push_back(c_szEndFlag);

        if(m_pSerialPort_control && m_pSerialPort_control->isOpen())
		{
            m_pSerialPort_control->write(szData);
		}

		m_bUpdatedX = false;
		m_bUpdatedY = false;
        m_bUpdatedAngle = false;
	}
}

void MainWindow::changeSwitchState(unsigned char &szSwitch, int num, int state)
{
	//num对6取余，区间转化到1-6之间，6为0
	num %= 6;
	//通过余数获取标志位
	unsigned char szFlag = 0xFF;
	switch(num)
	{
	case 0: //6
		szFlag = flag6;
		break;
	case 1:
		szFlag = flag1;
		break;
	case 2:
		szFlag = flag2;
		break;
	case 3:
		szFlag = flag3;
		break;
	case 4:
		szFlag = flag4;
		break;
	case 5:
		szFlag = flag5;
		break;
	default:
		return;
		break;
	}
	//
	if(state == 0)
	{
		szSwitch &= szFlag;
	}
	else if(state == 1)
	{
		szSwitch |= (~szFlag);
	}
	else
	{
		Q_ASSERT(false);
    }
}

void MainWindow::initSerialPort()
{
    //陀螺仪//
    m_pSerialPort_gyroscope = new QSerialPort(this);
    connect(m_pSerialPort_gyroscope, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
        this, &MainWindow::handleError);
    connect(m_pSerialPort_gyroscope, &QSerialPort::readyRead, this, &MainWindow::readData_gyroscope);

    m_pSettingsWidget_gyroscope = new SettingsWidget;
    connect(m_pSettingsWidget_gyroscope, SIGNAL(openSerialPort()), SLOT(openSerialPort_gyroscope()));
    connect(m_pSettingsWidget_gyroscope, SIGNAL(closeSerialPort()),SLOT(closeSerialPort()));
    QHBoxLayout *pHLayout_1 = new QHBoxLayout(ui->groupBox_1);
    pHLayout_1->addWidget(m_pSettingsWidget_gyroscope);

    //控制器//
    m_pSerialPort_control = new QSerialPort(this);
    connect(m_pSerialPort_control, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
        this, &MainWindow::handleError);
    connect(m_pSerialPort_control, &QSerialPort::readyRead, this, &MainWindow::readData_control);

    m_pSettingsWidget_control = new SettingsWidget;
    connect(m_pSettingsWidget_control, SIGNAL(openSerialPort()), SLOT(openSerialPort_control()));
    connect(m_pSettingsWidget_control, SIGNAL(closeSerialPort()),SLOT(closeSerialPort()));
    QHBoxLayout *pHLayout_2 = new QHBoxLayout(ui->groupBox_2);
    pHLayout_2->addWidget(m_pSettingsWidget_control);

    //激光阵列X
    m_pSerialPort_X = new QSerialPort(this);
    connect(m_pSerialPort_X, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
        this, &MainWindow::handleError);
    connect(m_pSerialPort_X, &QSerialPort::readyRead, this, &MainWindow::readData_X);

    m_pSettingsWidget_X = new SettingsWidget;
    connect(m_pSettingsWidget_X, SIGNAL(openSerialPort()), SLOT(openSerialPort_X()));
    connect(m_pSettingsWidget_X, SIGNAL(closeSerialPort()),SLOT(closeSerialPort()));
    QHBoxLayout *pHLayout_X = new QHBoxLayout(ui->groupBox_3);
    pHLayout_X->addWidget(m_pSettingsWidget_X);

    //激光阵列Y
    m_pSerialPort_Y = new QSerialPort(this);
    connect(m_pSerialPort_Y, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
        this, &MainWindow::handleError);
    connect(m_pSerialPort_Y, &QSerialPort::readyRead, this, &MainWindow::readData_Y);

    m_pSettingsWidget_Y = new SettingsWidget;
    connect(m_pSettingsWidget_Y, SIGNAL(openSerialPort()), SLOT(openSerialPort_Y()));
    connect(m_pSettingsWidget_Y, SIGNAL(closeSerialPort()),SLOT(closeSerialPort()));
    QHBoxLayout *pHLayout_Y = new QHBoxLayout(ui->groupBox_4);
    pHLayout_Y->addWidget(m_pSettingsWidget_Y);
}

void MainWindow::initTimer()
{
    m_pTimer_delay1 = new QTimer(this);
    connect(m_pTimer_delay1, SIGNAL(timeout()), this, SLOT(timeout_delayTimer1()));

    m_pTimer_delay2 = new QTimer(this);
    connect(m_pTimer_delay2, SIGNAL(timeout()), this, SLOT(timeout_delayTimer2()));

    m_pTimer_delay3 = new QTimer(this);
    connect(m_pTimer_delay3, SIGNAL(timeout()), this, SLOT(timeout_delayTimer3()));
}


