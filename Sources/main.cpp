#include "mainwindow.h"
#include <QApplication>
#include <QSplashScreen>
#include <QStyleFactory>


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setStyle(QStyleFactory::create("Fusion"));

    QPixmap pixmap(":/Images/splash.png");
    QSplashScreen splash(pixmap, Qt::WindowStaysOnTopHint);
    splash.show();
    app.processEvents();

    MainWindow window;
    window.show();
    splash.finish(&window);

    return app.exec();
}

