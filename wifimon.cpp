#include "wifimon.h"
#include "ui_wifimon.h"
#include <QProcess>
#include <QSettings>
#include <QDir>
#include <QDateTime>

// ////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////
WifiMon::WifiMon(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WifiMon)
{
    QSettings settings("dafinalilea","WifiMon");
    errorCount = 0;
    quit       = false;

    ui->setupUi(this);
    this->ui->scanTime->setValue(settings.value("timings/scantime",100).toInt());
    this->ui->downTime->setValue(settings.value("timings/downtime",500).toInt());
    this->ui->upTime->setValue(settings.value("timings/uptime",500).toInt());
    this->ui->ip->setText(settings.value("settings/ip","8.8.8.8").toString());
    trayIcon = new QSystemTrayIcon();

    trayMenu   = new QMenu();
    showAction = new QAction("Config",this);
    quitAction = new QAction("Exit",this);

    connect(showAction,SIGNAL(triggered(bool)),this,SLOT(show()));
    connect(quitAction,SIGNAL(triggered(bool)),this,SLOT(doQuit()));
    trayMenu->addAction(showAction);
    trayMenu->addAction(quitAction);
    trayIcon->setContextMenu(trayMenu);
    trayIcon->setIcon(QIcon(":/wifi-on.png"));
    trayIcon->show();

    scanTimer = new QTimer();
    downTimer = new QTimer();
    upTimer   = new QTimer();

    connect(scanTimer,SIGNAL(timeout()),this,SLOT(doScan()));
    connect(downTimer,SIGNAL(timeout()),this,SLOT(doDown()));
    connect(upTimer,SIGNAL(timeout()),this,SLOT(doUp()));

    scanTimer->setInterval(this->ui->scanTime->value());
    downTimer->setInterval(this->ui->downTime->value());
    upTimer->setInterval(this->ui->upTime->value());

    scanTimer->setSingleShot(true);
    downTimer->setSingleShot(true);
    upTimer->setSingleShot(true);
    QDir d;
    if(!d.exists("C:/ProgramData/WifiMon"))
        d.mkpath("C:/ProgramData/WifiMon");
    log.setFileName("C:/ProgramData/WifiMon/issues.log");
    log.open(QFile::ReadWrite);
    log.seek(log.size());
    scanTimer->start();
}

// ////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////
void WifiMon::logIssue(QString issue)
{
    if(log.size()>1024*1024*10){
        log.close();
        log.remove("C:/ProgramData/WifiMon/issues.log.old");
        log.rename("C:/ProgramData/WifiMon/issues.log.old");
        log.setFileName("C:/ProgramData/WifiMon/issues.log");
        log.open(QFile::ReadWrite);
    }
    QString txt=QDateTime::currentDateTime().toString("YYYY-MM-DD-hh:mm:ss.zzz")+" "+issue+"\n";
    log.write(txt.toUtf8());
}

// ////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////
void WifiMon::closeEvent(QCloseEvent *event)
{
        hide();
        if(!quit)event->ignore();
}

// ////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////
bool ping(const QString & ip)
{
    QProcess p;
    p.start(QString("ping.exe -n 1 -w 1 ")+ip);
    if(!p.waitForFinished(500)){
        p.kill();
    }
    int status = p.exitCode();
    return 0 == status;
}

// ////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////
QString getSSID()
{
    QProcess p;
    p.setReadChannel(QProcess::StandardOutput);
    p.start("netsh wlan show interfaces");
    p.waitForReadyRead();
    while(p.canReadLine()){
        QString s = QString::fromLocal8Bit(p.readLine());
        if(s.contains(" SSID ")){
            return s.split(":").last().trimmed();
        }
        p.waitForReadyRead();
    }
    return "";
}

// ////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////
void WifiMon::doScan()
{

    if(ssid=="")
    {
        ssid=getSSID();
        if(ssid!="") {
            logIssue(ssid+" online");
        } else {
            scanTimer->start();
            return;
        }
    }
    if( !ping(ui->ip->text())) {
        ssid=getSSID();
        if(ssid!="") {
            if(errorCount>1){
                trayIcon->setIcon(QIcon(":/wifi-issue.png"));
                logIssue(ssid+errorCount+" issue");
            }
        } else {
            logIssue(QString("Wifi ")+ssid+" offline");
            trayIcon->setIcon(QIcon(":/wifi-off.png"));
        }
        if(ssid!="" && errorCount++ > 3){
            downTimer->start();
            return;
        }
    } else {
        if (errorCount!= 0) trayIcon->setIcon(QIcon(":/wifi-on.png"));
        errorCount = 0;
    }
    scanTimer->start();
}

// ////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////
void WifiMon::doDown()
{
    QProcess p;
    p.execute("netsh wlan disconnect");
    logIssue(ssid+" down");
    upTimer->start();
}

// ////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////
void WifiMon::doUp()
{
    QProcess p;
    p.execute(QString("netsh wlan connect name=")+ssid);
    logIssue(ssid+" up");
    scanTimer->start();
}

// ////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////
void WifiMon::doQuit()
{
    quit = true;
    close();
}

// ////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////
WifiMon::~WifiMon()
{
    QSettings settings("dafinalilea","WifiMon");
    settings.setValue("timings/scantime",this->ui->scanTime->value());
    settings.setValue("timings/downtime",this->ui->downTime->value());
    settings.setValue("timings/uptime",this->ui->upTime->value());
    settings.setValue("settings/ip",this->ui->ip->text());
    trayIcon->hide();
    delete ui;
}
