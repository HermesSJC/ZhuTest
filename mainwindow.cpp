#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //刷新串口
    on_refreshSerialButton_clicked();

    //增加波特率
    ui->bandRateComboBox->clear();
    ui->bandRateComboBox->addItem("9600",QSerialPort::Baud9600);
    ui->bandRateComboBox->addItem("38400",QSerialPort::Baud38400);
    ui->bandRateComboBox->addItem("57600",QSerialPort::Baud57600);
    ui->bandRateComboBox->addItem("115200",QSerialPort::Baud115200);
    ui->bandRateComboBox->setCurrentIndex(3);

    //增加时间间隔
    ui->intervalTimeComboBox->clear();
    ui->intervalTimeComboBox->addItem("200",200);
    ui->intervalTimeComboBox->addItem("500",500);
    ui->intervalTimeComboBox->addItem("1000",1000);
    ui->intervalTimeComboBox->setCurrentIndex(0);

    nSizeNum = 0;
    nTxIndex = 0;

    isShowTimeFlag = true;

    connect(&serialPort,SIGNAL(readyRead()),this,SLOT(on_serialPort_readyRead()));

    //增加系统标置位
    ui->statusBar->showMessage(GetCurrentTime() + " - " + QString::fromLocal8Bit("系统初始化完毕!"),1000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_connectSerialButton_clicked()
{
    //设置串口
    serialPort.setPortName(ui->portNameComboBox->currentText());
    serialPort.setBaudRate(static_cast<QSerialPort::BaudRate>(ui->bandRateComboBox->currentData().toInt()));
    serialPort.setDataBits(QSerialPort::Data8);
    serialPort.setParity(QSerialPort::NoParity);
    serialPort.setStopBits(QSerialPort::OneStop);

    //尝试打开串口
    if(serialPort.open(QIODevice::ReadWrite))
    {
        ui->bandRateComboBox->setDisabled(true);
        ui->portNameComboBox->setDisabled(true);
        ui->disconnectSerialButton->setEnabled(true);
        ui->intervalTimeComboBox->setDisabled(true);
        ui->connectSerialButton->setDisabled(true);
        ui->refreshSerialButton->setDisabled(true);
        ui->statusBar->showMessage(GetCurrentTime() + " - " + QString::fromLocal8Bit("串口打开成功!"),1000);
    }
    else
    {
        ui->statusBar->showMessage(GetCurrentTime() + " - " + QString::fromLocal8Bit("串口打开失败!"),1000);
    }
}

void MainWindow::on_disconnectSerialButton_clicked()
{
    //关闭
    serialPort.close();

    //如果还打开,报错
    if(serialPort.isOpen())
    {
        ui->statusBar->showMessage(GetCurrentTime() + " - " + QString::fromLocal8Bit("串口关闭失败!"),1000);
        return;
    }

    ui->bandRateComboBox->setEnabled(true);
    ui->portNameComboBox->setEnabled(true);
    ui->connectSerialButton->setEnabled(true);
    ui->disconnectSerialButton->setDisabled(true);
    ui->intervalTimeComboBox->setEnabled(true);
    ui->refreshSerialButton->setEnabled(true);
    ui->statusBar->showMessage(GetCurrentTime() + " - " + QString::fromLocal8Bit("串口打开成功!"),1000);


}

void MainWindow::on_refreshSerialButton_clicked()
{
    //清除原有内容
    ui->portNameComboBox->clear();

    //添加现有串口
    const auto infos = QSerialPortInfo::availablePorts();
    //如果是空的就退出
    if(infos.isEmpty())
    {
        ui->portNameComboBox->addItem("None");
        return;
    }

    //非空就添加串口
    for(const QSerialPortInfo &info: infos)
    {
        ui->portNameComboBox->addItem(info.portName());
    }
}

//void MainWindow::on_chooseInitPathButton_clicked()
//{
//    QString filePath = QFileDialog::getOpenFileName(this,QString::fromLocal8Bit("选择文件"),currentPath.path(),tr("* txt"));
//    if(filePath.isEmpty())
//    {
//        return;
//    }

//    sendInitData.clear();
//    QFile file(filePath);
//    QString line;
//    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
//    {
//        //没到底就一直读
//        while(!file.atEnd())
//        {
//            line = QString(file.readLine());
//            int nIndexJ = line.indexOf('J');
//            if(nIndexJ != -1)
//            {
//                line.remove("\r");
//                line.remove("\n");
//                line.remove(0,nIndexJ);
//                sendInitData.append(line);
//            }

//        }
//        nSizeInit = sendInitData.size();
//        ui->txLineEdit->setText(sendInitData.at(0));
//        ui->initNumLabel->setText(QString::fromLocal8Bit("初始路径共%1个点").arg(nSizeInit));
//    }

//}


void MainWindow::on_intervalTimeComboBox_activated(int index)
{

}

//void MainWindow::on_continuousTxInitButton_toggled(bool checked)
//{
//    //判断串口是否打开
//    if(serialPort.isOpen() == false)
//    {
//        ui->statusBar->showMessage(GetCurrentTime() + " - " + QString::fromLocal8Bit("请先打开串口!"),1000);
//        ui->continuousTxInitButton->setChecked(false);
//        return;
//    }

//    //判断是否有足够的点
//    if(nSizeInit == 0 )
//    {
//        ui->statusBar->showMessage(GetCurrentTime() + " - " + QString::fromLocal8Bit("没有足够的点!"),1000);
//        ui->continuousTxInitButton->setChecked(false);
//        return ;
//    }

//    //按下
//    if(checked == true)
//    {
//        nTimerIDInit = startTimer(ui->intervalTimeComboBox->currentData().toInt());
//        isSendInit= true;
//    }
//    //松开
//    else
//    {
//        killTimer(nTimerIDInit);
//        isSendInit = false;
//    }
//}



void MainWindow::on_clearRxTxButton_clicked()
{
    ui->txLineEdit->clear();
    ui->txNextLineEdit->clear();

    ui->rxTextEdit->clear();
    ui->rxTextEdit->document()->clear();
}

void MainWindow::on_singleTxButton_clicked()
{
    //判断串口是否打开
    if(serialPort.isOpen() == false)
    {
        ui->statusBar->showMessage(GetCurrentTime() + " - " + QString::fromLocal8Bit("请先打开串口"),1000);
        return;
    }

    if(ui->txLineEdit->text().isEmpty()==true)
    {
        ui->statusBar->showMessage(GetCurrentTime() + " - " + QString::fromLocal8Bit("发送栏没有内容"),1000);
        return;
    }

    //发送当前对话框的
    serialPort.write(ui->txLineEdit->text().toLatin1().append("\r\n"));

}

void MainWindow::on_serialPort_readyRead()
{
    QTextCursor textCursor = ui->rxTextEdit->textCursor();
    textCursor.movePosition(QTextCursor::End);
    ui->rxTextEdit->setTextCursor(textCursor);

    QString readData = QString(serialPort.readAll());
    isShowTimeFlag ? readData = readData.replace("\r\n",GetCurrentTime() + " - \r\n") : readData;

    ui->rxTextEdit->insertPlainText(QString(serialPort.readAll()));
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    //    //判断是不是初始
    //    if(event->timerId() == nTimerIDInit && isSendInit == true)
    //    {
    //        //有就发送
    //        if(nIndexInit < nSizeInit)
    //        {
    //            //发送数据
    //            ui->statusBar->showMessage(GetCurrentTime() + " - " + QString::fromLocal8Bit("当前发送第%1个").arg(nIndexInit),1000);
    //            serialPort.write(sendInitData.at(nIndexInit).toLatin1().append("\r\n"));

    //            //如果是最后一个
    //            if(nIndexInit == nSizeInit-1)
    //            {
    //                //复位清零
    //                nIndexInit = 0;
    //                ui->txLineEdit->clear();

    //                emit ui->continuousTxInitButton->toggled(false);
    //                ui->continuousTxInitButton->setChecked(false);
    //            }
    //            //不是最后一个就把下一个坐标放在文本上
    //            else
    //            {
    //                nIndexInit ++;
    //                ui->txLineEdit->setText(sendInitData.at(nIndexInit));
    //            }
    //        }
    //        else
    //        {
    //            ui->statusBar->showMessage(GetCurrentTime() + " - " + QString::fromLocal8Bit("发送完毕"),1000);
    //        }
    //    }



}



void MainWindow::on_isShowTimeCheckBox_stateChanged(int arg1)
{
    (arg1 == 0) ? isShowTimeFlag = false : isShowTimeFlag = true;
}



void MainWindow::on_chooseFileButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this,QString::fromLocal8Bit("选择文件"),currentPath.path(),tr("* txt"));
    if(filePath.isEmpty())
    {
        return;
    }

    readData.clear();
    nTxIndex = 1;

    QFile file(filePath);
    QString line;
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        //没到底就一直读
        while(!file.atEnd())
        {
            line = QString(file.readLine());
            if(line.isEmpty() == false)
            {
                line.remove('\n');
                readData.append(line);
            }
        }
        nSizeNum = readData.size();
        if(nSizeNum == 0)
        {
            ui->fileNumLabel->setText(QString::fromLocal8Bit("没有读取到数据"));
        }
        else if(nSizeNum == 1)
        {
            ui->txLineEdit->setText(readData.at(0));
        }
        else
        {
            ui->txLineEdit->setText(readData.at(0));
            ui->txNextLineEdit->setText(readData.at(1));
        }
        ui->fileNumLabel->setText(QString::fromLocal8Bit("文件共%1行").arg(nSizeNum));
    }
}


void MainWindow::on_resetTxButton_clicked()
{
    nTxIndex = 1;
    if(nSizeNum == 0)
    {
        ui->fileNumLabel->setText(QString::fromLocal8Bit("没有读取到数据"));
    }
    else if(nSizeNum == 1)
    {
        ui->txLineEdit->setText(readData.at(0));
    }
    else
    {
        ui->txLineEdit->setText(readData.at(0));
        ui->txNextLineEdit->setText(readData.at(1));
    }
    ui->statusBar->showMessage(GetCurrentTime() + " - " + QString::fromLocal8Bit("初始发送成功"),1000);
}

void MainWindow::on_nextTxButton_clicked()
{
    //判断串口是否打开
    if(serialPort.isOpen() == false)
    {
        ui->statusBar->showMessage(GetCurrentTime() + " - " + QString::fromLocal8Bit("请先打开串口"),1000);
        return;
    }

    if(ui->txLineEdit->text().isEmpty() == true)
    {
        ui->statusBar->showMessage(GetCurrentTime() + " - " + QString::fromLocal8Bit("发送栏没有内容"),1000);
        return;
    }

    //没有连续发送
    if(nSizeNum == 0)
    {
        ui->statusBar->showMessage(GetCurrentTime() + " - " + QString::fromLocal8Bit("没有连续发送内容"),1000);
        return;
    }
    //只有一个内容
    else if(nSizeNum == 1)
    {
        serialPort.write(ui->txLineEdit->text().toLatin1().append("\r\n"));
        ui->txLineEdit->clear();
    }
    //有至少2个内容
    else
    {
        //如果是最后一个
        if(nTxIndex == nSizeNum)
        {
            serialPort.write(ui->txLineEdit->text().toLatin1().append("\r\n"));
            ui->txLineEdit->clear();
            ui->txNextLineEdit->clear();
        }
        //如果是倒数第二个
        else if(nTxIndex == nSizeNum-1)
        {
            //发送当前对话框的
            serialPort.write(ui->txLineEdit->text().toLatin1().append("\r\n"));
            //读取下一个对话框的
            QString next = ui->txNextLineEdit->text();
            //把下一个对话框的内容放在这个对话框
            ui->txLineEdit->setText(next);
            //下一个对话框清零
            ui->txNextLineEdit->clear();
            //下标自增
            nTxIndex ++;
        }
        //没到最后一个就循环发送
        else
        {
            //发送当前对话框的
            serialPort.write(ui->txLineEdit->text().toLatin1().append("\r\n"));
            //读取下一个对话框的
            QString next = ui->txNextLineEdit->text();
            //把下一个对话框的内容放在这个对话框
            ui->txLineEdit->setText(next);
            //下一个对话框放上下一个内容
            ui->txNextLineEdit->setText(readData.at(nTxIndex+1));
            nTxIndex ++;
        }
    }
}

void MainWindow::on_continuousTxButton_clicked(bool checked)
{
    if(checked == true)
    {
        ui->statusBar->showMessage(GetCurrentTime() + " - " + QString::fromLocal8Bit("还没有这个功能呢"),1000);
    }
}

