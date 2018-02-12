#ifndef DETACHWINDOW_H
#define DETACHWINDOW_H

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
    DetachWindow(QWidget *paneWidget, QString paneName, Transparency transparency, const QPoint &dropPoint = QPoint());
    ~DetachWindow();


//Variables
private:
    QPushButton *closeButton = NULL;
    ResizeButton *resizeButton = NULL;
    QPoint dragPosition;
    QString paneName;
    QWidget *paneWidget;


//Metodos
private:
    void completeUI(Transparency transparency);
    void completeUIButtons();
    void moveInScreen(QPoint pos, QSize size);
    void readSettings();
    void showWindowFrame(bool showFrame);
    void writeSettings();
    void resizeChecks();
    void updateButtonsTheme();
    void moveResize(const QPoint &pos, const QSize &size);

//Override events
protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
//    void changeEvent(QEvent *event) Q_DECL_OVERRIDE;
//    void leaveEvent(QEvent *e) Q_DECL_OVERRIDE;
//    void enterEvent(QEvent *e) Q_DECL_OVERRIDE;

public:

signals:
    void resized();
    void closed(DetachWindow *detachWindow, QWidget *paneWidget);
    void pLog(QString line);
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="DetachWindow");

private slots:
    void resizeSlot(QSize size);
};


#endif // DETACHWINDOW_H
