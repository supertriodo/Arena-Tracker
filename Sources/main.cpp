#include "mainwindow.h"
#include <QApplication>
#include <QSplashScreen>
#include <QStyleFactory>


bool isNewApp()
{
    QString runningBinaryName = QCoreApplication::applicationFilePath().split("/").last();
    return runningBinaryName == "ArenaTracker.new";
}


int newAppMoveRestart()
{
    QSettings settings("Arena Tracker", "Arena Tracker");
    QString runningBinaryPath = settings.value("runningBinaryPath", "").toString();
    QString runningBinaryName = runningBinaryPath.split("/").last();
    QString dataBinaryPath = Utility::dataPath() + "/" + runningBinaryName;

    qDebug() << "ArenaTracker.new running...";
    qDebug() << "remove" << runningBinaryPath;

    QFile appFile(runningBinaryPath);
    if(!appFile.exists())   return 0;
    while(!appFile.remove())    QThread::sleep(1);

    qDebug() << dataBinaryPath << "rename" << runningBinaryPath;

    QFile appDataFile(dataBinaryPath);
    while(!appDataFile.rename(runningBinaryPath))    QThread::sleep(1);

    qDebug() << "Start moved ArenaTracker...";
    QProcess::startDetached(runningBinaryPath, QStringList());

    return 1;
}


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setStyle(QStyleFactory::create("Fusion"));

    if(isNewApp())   return newAppMoveRestart();

    QPixmap pixmap(":/Images/splash.png");
    QSplashScreen splash(pixmap, Qt::WindowStaysOnTopHint);
    splash.show();
    app.processEvents();

    MainWindow window;
    window.show();
    splash.finish(&window);

    return app.exec();
}

