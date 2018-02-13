#include "detachwindow.h"
#include "themehandler.h"
#include <QtWidgets>


DetachWindow::DetachWindow(QWidget *paneWidget, QString paneName, Transparency transparency, const QPoint& dropPoint) :
    QMainWindow(0, Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint)
{
    this->paneName = paneName;
    this->paneWidget = paneWidget;
    completeUI(transparency);
    readSettings(dropPoint);
    show();
}


DetachWindow::~DetachWindow()
{
}


void DetachWindow::completeUI(Transparency transparency)
{
    this->setWindowIcon(QIcon(":/Images/icon.png"));
    this->setWindowTitle("AT " + paneName);

    paneWidget->setParent(this);
    setCentralWidget(paneWidget);
    paneWidget->show();

    completeUIButtons();

    this->setAttribute(Qt::WA_TranslucentBackground, transparency!=Framed);
    this->showWindowFrame(transparency == Framed);
}


void DetachWindow::completeUIButtons()
{
    closeButton = new QPushButton("", this);
    closeButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    closeButton->resize(24, 24);
    closeButton->setIconSize(QSize(24, 24));
    closeButton->setFlat(true);
    connect(closeButton, SIGNAL(clicked()),
            this, SLOT(close()));

    resizeButton = new ResizeButton(this);
    resizeButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    resizeButton->resize(24, 24);
    resizeButton->setIconSize(QSize(24, 24));
    resizeButton->setFlat(true);
    connect(resizeButton, SIGNAL(newSize(QSize)),
            this, SLOT(resizeSlot(QSize)));

    Utility::fadeOutWidget(closeButton);
}


void DetachWindow::readSettings(const QPoint& dropPoint)
{
    QSettings settings("Arena Tracker", "Arena Tracker");
    QPoint pos;
    QSize size;

    if(dropPoint.isNull())  pos = settings.value("posWindow" + paneName, QPoint(0,0)).toPoint();
    else                    pos = dropPoint;
    size = settings.value("sizeWindow" + paneName, QSize(255, 600)).toSize();

    moveResize(pos, size);
}


void DetachWindow::moveResize(const QPoint &pos, const QSize &size)
{
    this->show();
    this->setMinimumSize(100,200);  //El minimumSize inicial es incorrecto
    resize(size);
    moveInScreen(pos, size);
}


void DetachWindow::writeSettings()
{
    QSettings settings("Arena Tracker", "Arena Tracker");
    settings.setValue("posWindow" + paneName, pos());
    settings.setValue("sizeWindow" + paneName, size());
}


void DetachWindow::moveInScreen(QPoint pos, QSize size)
{
    QRect appRect(pos, size);
    QPoint midPoint = appRect.center();

    QString message =
            "Window Pos: (" + QString::number(pos.x()) + "," + QString::number(pos.y()) +
            ") - Size: (" + QString::number(size.width()) + "," + QString::number(size.height()) +
            ") - Mid: (" + QString::number(midPoint.x()) + "," + QString::number(midPoint.y()) + ")";
    emit pDebug(message);

    foreach(QScreen *screen, QGuiApplication::screens())
    {
        if (!screen)    continue;
        QRect geometry = screen->geometry();

        if(geometry.contains(midPoint))
        {
            message =
                    "Window in screen: (" + QString::number(geometry.left()) + "," + QString::number(geometry.top()) + "," +
                    QString::number(geometry.right()) + "," + QString::number(geometry.bottom()) + ")";
            emit pDebug(message);
            move(pos);
            return;
        }
    }

    message = "Window outside screens. Move to (0,0)";
    emit pDebug(message);
    move(QPoint(0,0));
}


void DetachWindow::showWindowFrame(bool showFrame)
{
    if(showFrame)
    {
        this->setWindowFlags(Qt::Window|Qt::WindowStaysOnTopHint);
    }
    else
    {
        this->setWindowFlags(Qt::Window|Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
    }
    this->show();
}


void DetachWindow::spreadTheme()
{
    updateButtonsTheme();
    resizeChecks();//Recoloca boton X
}


void DetachWindow::updateButtonsTheme()
{
    closeButton->setStyleSheet("QPushButton {background: " + ThemeHandler::bgTopButtonsColor() + "; border: none;}"
                                   "QPushButton:hover {background: " + ThemeHandler::hoverTopButtonsColor() + ";}");
    closeButton->setIcon(QIcon(ThemeHandler::buttonCloseFile()));

    resizeButton->setIcon(QIcon(ThemeHandler::buttonResizeFile()));
}


void DetachWindow::resizeSlot(QSize size)
{
    resize(size);
}


void DetachWindow::resizeChecks()
{
    QWidget *widget = this->centralWidget();

    int top = widget->pos().y();
    int bottom = top + widget->height();
    int left = widget->pos().x();
    int right = left + widget->width();

    closeButton->move(right - 24 - ThemeHandler::borderWidth(), top + ThemeHandler::borderWidth());
    resizeButton->move(right - 24, bottom - 24);
}


void DetachWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    emit resized();

    resizeChecks();
    event->accept();
}


void DetachWindow::closeEvent(QCloseEvent *event)
{
    QMainWindow::closeEvent(event);
    emit closed(this, paneWidget);

    writeSettings();
    event->accept();
}


void DetachWindow::leaveEvent(QEvent * e)
{
    QMainWindow::leaveEvent(e);

    Utility::fadeOutWidget(closeButton);
}


void DetachWindow::enterEvent(QEvent * e)
{
    QMainWindow::enterEvent(e);

    Utility::fadeInWidget(closeButton);
}


void DetachWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}


void DetachWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        QPoint newPosition = event->globalPos() - dragPosition;
        int top = newPosition.y();
        int bottom = top + this->height();
        int left = newPosition.x();
        int right = left + this->width();
        int midX = (left + right)/2;
        int midY = (top + bottom)/2;

        const int stickyMargin = 10;

        foreach (QScreen *screen, QGuiApplication::screens())
        {
            if (!screen)    continue;
            QRect screenRect = screen->geometry();
            int topScreen = screenRect.y();
            int bottomScreen = topScreen + screenRect.height();
            int leftScreen = screenRect.x();
            int rightScreen = leftScreen + screenRect.width();

            if(midX < leftScreen || midX > rightScreen ||
                    midY < topScreen || midY > bottomScreen) continue;

            if(std::abs(top - topScreen) < stickyMargin)
            {
                newPosition.setY(topScreen);
            }
            else if(std::abs(bottom - bottomScreen) < stickyMargin)
            {
                newPosition.setY(bottomScreen - this->height());
            }
            if(std::abs(left - leftScreen) < stickyMargin)
            {
                newPosition.setX(leftScreen);
            }
            else if(std::abs(right - rightScreen) < stickyMargin)
            {
                newPosition.setX(rightScreen - this->width());
            }
            move(newPosition);
            event->accept();
            return;
        }

        move(newPosition);
        event->accept();
    }
}


//Restaura ambas ventanas minimizadas
//void DetachWindow::changeEvent(QEvent * event)
//{
//    if(event->type() == QEvent::WindowStateChange)
//    {
//        if((windowState() & Qt::WindowMinimized) == 0)
//        {
//            if(this->mainWindow != NULL)
//            {
//                this->mainWindow->setWindowState(Qt::WindowActive);
//            }
//        }

//    }
//}
