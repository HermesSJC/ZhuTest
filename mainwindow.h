#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QTimer>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDateTime>
#include <QFile>
#include <QFileDialog>
#include <QTimerEvent>
#include <QDir>

#include <QDebug>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    QSerialPort serialPort;

    QDateTime dateTime;
    inline QString GetCurrentTime(QString format);

    int nSizeNum;
    int nTxIndex;
    QList<QString> readData;

    QDir currentPath;

    bool isShowTimeFlag;


private slots:

    void on_connectSerialButton_clicked();

    void on_disconnectSerialButton_clicked();

    void on_intervalTimeComboBox_activated(int index);

    void on_clearRxTxButton_clicked();

    void on_singleTxButton_clicked();

    void on_serialPort_readyRead();

    void on_isShowTimeCheckBox_stateChanged(int arg1);

    void on_refreshSerialButton_clicked();

    void on_chooseFileButton_clicked();

    void on_resetTxButton_clicked();

    void on_nextTxButton_clicked();

    void on_continuousTxButton_clicked(bool checked);

protected:
    void timerEvent(QTimerEvent *event);

private:
    Ui::MainWindow *ui;
};


inline QString MainWindow::GetCurrentTime(QString format = "hh:mm:ss")
{
    return dateTime.currentDateTime().toString(format);
}





#endif // MAINWINDOW_H
