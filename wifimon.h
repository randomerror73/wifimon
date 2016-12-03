#ifndef WIFIMON_H
#define WIFIMON_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QCloseEvent>
#include <QTimer>
#include <QMenu>
#include <QFile>

namespace Ui {
class WifiMon;
}

class WifiMon : public QMainWindow
{
    Q_OBJECT

public:
    explicit WifiMon(QWidget *parent = 0);
    ~WifiMon();

    void closeEvent(QCloseEvent *event);

private slots:

    void doScan();
    void doDown();
    void doUp();
    void doQuit();

private:
    void logIssue(QString issue);
    Ui::WifiMon     * ui;

    QSystemTrayIcon * trayIcon;

    QTimer          * scanTimer;
    QTimer          * downTimer;
    QTimer          * upTimer;

    bool              quit;

    QMenu           * trayMenu;

    QAction         * showAction;
    QAction         * quitAction;

    unsigned          errorCount;

    QString           ssid;
    QFile             log;

};

#endif // WIFIMON_H
