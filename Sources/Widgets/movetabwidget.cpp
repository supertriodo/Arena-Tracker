#include "movetabwidget.h"
#include "../themehandler.h"
#include <QtWidgets>

MoveTabWidget::MoveTabWidget(QWidget *parent) : QTabWidget(parent)
{
    MoveTabBar *moveTabBar = new MoveTabBar(this);
    this->setTabBar(moveTabBar);
    this->hide();
    this->tabSize = 32;

    connect(moveTabBar, SIGNAL(detachTab(int,QPoint)),
            this, SIGNAL(detachTab(int,QPoint)));
}


void MoveTabWidget::setTheme(QString tabBarAlignment, int maxWidth, bool resizing, bool transparent)
{
    QString bgTabsColor = ThemeHandler::bgTabsColor();
    QString hoverTabsColor = ThemeHandler::hoverTabsColor();
    QString selectedTabsColor = ThemeHandler::selectedTabsColor();
    QString borderWidth = "";
    if(ThemeHandler::borderWidth() > 0) borderWidth = QString::number(ThemeHandler::borderWidth());
    int tabSize = std::max(24, std::min(32, maxWidth/std::max(1, this->count()) - 14));

    if(resizing && (tabSize == this->tabSize)) return;

    QString css =
        "QTabBar::tab:selected {background: " + selectedTabsColor + "; border-bottom-color: " + selectedTabsColor + ";}"
        "QTabBar::tab:hover {background: " + hoverTabsColor + "; border-bottom-color: " + hoverTabsColor + ";}"
        "QTabWidget::pane { " + ThemeHandler::bgApp() + ThemeHandler::borderApp(transparent) +
            "position: absolute; top: -38px;}"
        "QTabBar::tab {border: 2px solid " + bgTabsColor + ";"
            "padding: 5px;background: " + bgTabsColor +
            "; height: 24px; width: " + QString::number(tabSize) + "px}";

    if(borderWidth.isEmpty())   css +=  "QTabWidget::tab-bar {alignment: " + tabBarAlignment + ";}";
    else                        css +=  "QTabWidget::tab-bar {left: " + borderWidth + "px; "
                                            "top: " + borderWidth + "px;}";

    this->setStyleSheet(css);

    if(tabSize != this->tabSize)    tabBar()->setIconSize(QSize(tabSize, tabSize));
    this->tabSize = tabSize;
}



MoveTabBar::MoveTabBar(QWidget *parent) : QTabBar(parent)
{
    setIconSize(QSize(32,32));
    setAcceptDrops(true);
//    setMovable(true);
}


void MoveTabBar::mousePressEvent(QMouseEvent* event)
{
  if (event->button() == Qt::LeftButton)
  {
    dragStartPos = event->pos();
//    dragLastPos = dragStartPos;
  }

  QTabBar::mousePressEvent(event);
}


void MoveTabBar::mouseMoveEvent(QMouseEvent* event)
{
    bool mouseLeft = ((event->buttons() & Qt::LeftButton));
    bool dragging = ((event->pos() - dragStartPos).manhattanLength() > QApplication::startDragDistance());
    bool mouseInWidget = (geometry().contains(event->pos()));
//    if(mouseInWidget)   dragLastPos = event->pos();
    if(mouseLeft && dragging && !mouseInWidget)
    {
        // Stop the move to be able to convert to a drag
        QMouseEvent* finishMoveEvent = new QMouseEvent (QEvent::MouseMove, event->pos (), Qt::NoButton, Qt::NoButton, Qt::NoModifier);
        QTabBar::mouseMoveEvent (finishMoveEvent);
        delete finishMoveEvent;
        finishMoveEvent = NULL;

        // Initiate Drag
        QDrag* drag = new QDrag(this);
        QMimeData* mimeData = new QMimeData;
        // a crude way to distinguish tab-reordering drops from other ones
        mimeData->setData("action", "application/tab-detach") ;
        drag->setMimeData(mimeData);


    // Create transparent screen dump
//    QPixmap pixmap = QGuiApplication::primaryScreen()->grabWindow
//            (dynamic_cast <MoveTabWidget*> (parentWidget())->currentWidget()->winId()).scaled(640, 480, Qt::KeepAspectRatio);
//    QPixmap targetPixmap (pixmap.size ());
//    QPainter painter (&targetPixmap);
//    painter.setOpacity (0.5);
//    painter.drawPixmap (0,0, pixmap);
//    painter.end ();
//    drag->setPixmap (targetPixmap);
//    drag->setHotSpot (QPoint (20, 10));

        // Handle Detach and Move
        Qt::DropAction dragged = drag->exec (Qt::MoveAction | Qt::CopyAction);
        if (Qt::IgnoreAction == dragged)
        {
            event->accept ();
            emit detachTab(tabAt(dragStartPos), QCursor::pos());
        }
        else if (Qt::MoveAction == dragged)
        {
            event->accept();
        }
    }
    else
    {
        QTabBar::mouseMoveEvent(event);
    }
}


void MoveTabBar::dragEnterEvent(QDragEnterEvent* event)
{
    const QMimeData* m = event->mimeData();
    QStringList formats = m->formats();
    if(formats.contains("action") && (m->data("action") == "application/tab-detach"))
    {
        event->acceptProposedAction();
    }
}


void MoveTabBar::dropEvent(QDropEvent* event)
{
    event->setAccepted(true);
}

