#include "movelistwidget.h"
#include "../Cards/deckcard.h"
#include "../themehandler.h"
#include "qevent.h"


PixelPerfectDelegate::PixelPerfectDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
}
void PixelPerfectDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QIcon icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));
    if (icon.isNull()) {
        return;
    }
    icon.paint(painter, option.rect);
}
QSize PixelPerfectDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    QIcon icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));
    if (icon.isNull()) {
        return QSize(0, 0);
    }
    return icon.actualSize(QSize(1024, 1024));
}



MoveListWidget::MoveListWidget(QWidget *parent) : QListWidget(parent)
{
    this->setFrameShape(QFrame::NoFrame);
    this->setIconSize(10*CARD_SIZE);
    this->setTheme();
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setSizeAdjustPolicy(QAbstractScrollArea::AdjustIgnored);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setSelectionMode(QAbstractItemView::NoSelection);
    this->setFocusPolicy(Qt::NoFocus);

    m_delegate = new PixelPerfectDelegate(this);
    this->setItemDelegate(m_delegate);
}


void MoveListWidget::setTheme()
{
    this->setStyleSheet(
        "QListView{background-color: transparent; outline: 0;}"
        "QListView::item{padding: 0px;}"
        "QListView::item:selected {background: " + ThemeHandler::bgSelectedItemListColor() + "; "
            "color: " + ThemeHandler::fgSelectedItemListColor() + ";}"
    );
}


void MoveListWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    QListWidget::mouseDoubleClickEvent(event);
    event->ignore();
}
void MoveListWidget::mouseMoveEvent(QMouseEvent *event)
{
    QListWidget::mouseMoveEvent(event);
    event->ignore();
}
void MoveListWidget::mousePressEvent(QMouseEvent *event)
{
    QListWidget::mousePressEvent(event);
    event->ignore();
}
void MoveListWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QListWidget::mouseReleaseEvent(event);
    event->ignore();
}


void MoveListWidget::leaveEvent(QEvent * e)
{
    QListWidget::leaveEvent(e);

    int mouseX = this->mapFromGlobal(QCursor::pos()).x();
    if(mouseX < 0 || mouseX >= size().width())  emit xLeave();
    emit leave();
}
