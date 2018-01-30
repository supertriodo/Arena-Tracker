#ifndef DETACHWINDOW_H
#define DETACHWINDOW_H

#include "mainwindow.h"
#include "Widgets/resizebutton.h"
#include "utility.h"
#include <QMainWindow>
#include <QPushButton>

class MainWindow;

class DetachWindow : public QMainWindow
{
    Q_OBJECT

friend class MainWindow;

//Constructor
public:
    DetachWindow(QWidget *parent, MainWindow *mainWindow);
    ~DetachWindow();


//Variables
private:
    QPushButton *minimizeButton = NULL;
    QPushButton *closeButton = NULL;
    ResizeButton *resizeButton = NULL;
    MainWindow *mainWindow;
    Transparency transparency;
    QPoint dragPosition;


//Metodos
private:
    void completeUI();
    void completeUIButtons();
    void moveInScreen(QPoint pos, QSize size);
    void readSettings();
    void showWindowFrame(bool showFrame);
    void writeSettings();
    void resizeChecks();
    void updateButtonsTheme();

//Override events
protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void changeEvent(QEvent *event) Q_DECL_OVERRIDE;
//    void leaveEvent(QEvent *e) Q_DECL_OVERRIDE;
//    void enterEvent(QEvent *e) Q_DECL_OVERRIDE;

signals:
    void pLog(QString line);
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="DetachWindow");

private slots:
    void resizeSlot(QSize size);
};


#endif // DETACHWINDOW_H
