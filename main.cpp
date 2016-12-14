#include "wifimon.h"
#include <QApplication>
#include <QSystemSemaphore>

class ReleaseSemaphore
{
public:
    ReleaseSemaphore(QSystemSemaphore & sem):semaphore(sem){}
    ~ReleaseSemaphore(){semaphore.release();}
    QSystemSemaphore & semaphore;
};

int main(int argc, char *argv[])
{
    //QSystemSemaphore semaphore("0cb49fdf-8058-47be-91ca-72d02e7f715d",1);
    //ReleaseSemaphore guard(semaphore);
    //if (!semaphore.acquire())return 0;
    QApplication a(argc, argv);
    WifiMon w;

    return a.exec();
}
