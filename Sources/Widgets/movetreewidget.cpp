#include "movetreewidget.h"
#include "../themehandler.h"
#include <QtWidgets>

MoveTreeWidget::MoveTreeWidget(QWidget *parent) : QTreeWidget(parent)
{
    this->setFrameShape(QFrame::NoFrame);
    setTheme(false);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setSizeAdjustPolicy(QAbstractScrollArea::AdjustIgnored);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setSelectionMode(QAbstractItemView::NoSelection);
    this->setFocusPolicy(Qt::NoFocus);
    this->setHeaderHidden(true);
    this->header()->close();
}


void MoveTreeWidget::setTheme(bool standAlone)
{
    QFont font(ThemeHandler::bigFont());
    font.setPixelSize(22);
    this->setFont(font);

    this->setStyleSheet("QTreeView{" + (standAlone?ThemeHandler::bgDecks()+ThemeHandler::borderDecks():ThemeHandler::bgWidgets()) +
                        "; outline: 0; padding-top: 5px;}"

    "QTreeView::branch:has-children:!has-siblings:closed,"
    "QTreeView::branch:closed:has-children:has-siblings{border-image: none;image: url(" + ThemeHandler::branchClosedFile() + ");}"

    "QTreeView::branch:open:has-children:!has-siblings,"
    "QTreeView::branch:open:has-children:has-siblings{border-image: none;image: url(" + ThemeHandler::branchOpenFile() + ");}"

    "QTreeView::item:selected {background: " + ThemeHandler::bgSelectedItemListColor() +
                        "; color: " + ThemeHandler::fgSelectedItemListColor() + ";}"
    "QTreeView::branch:selected {background-color: " + ThemeHandler::bgSelectedItemListColor() + "; }"
    );

    this->setAnimated(!standAlone);
}


void MoveTreeWidget::setEditableColumns(QList<int> cols)
{
    this->editableColumns = cols;
}


void MoveTreeWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    QTreeWidget::mouseDoubleClickEvent(event);
    event->ignore();
}
void MoveTreeWidget::mouseMoveEvent(QMouseEvent *event)
{
    QTreeWidget::mouseMoveEvent(event);
    event->ignore();
}
void MoveTreeWidget::mousePressEvent(QMouseEvent *event)
{
    if(!this->indexAt(event->pos()).isValid())
    {
        this->clearSelection();
    }
    QTreeWidget::mousePressEvent(event);
    event->ignore();
}
void MoveTreeWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QTreeWidget::mouseReleaseEvent(event);
    event->ignore();
}


void MoveTreeWidget::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Escape)
    {
        this->clearSelection();
    }
    QTreeWidget::keyPressEvent(event);
    event->ignore();
}


void MoveTreeWidget::leaveEvent(QEvent * e)
{
    QTreeWidget::leaveEvent(e);

    int mouseX = this->mapFromGlobal(QCursor::pos()).x();
    if(mouseX < 0 || mouseX >= size().width())  emit xLeave();
    emit leave();
}


QModelIndex MoveTreeWidget::moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
{
    if(cursorAction == QAbstractItemView::MoveNext)
    {
        QModelIndex index = this->currentIndex();
        if(index.isValid() && !editableColumns.isEmpty())
        {
            int nextCol = index.column();
            int nextRow = index.row();
            int cols = this->model()->columnCount();
            int rows = this->model()->rowCount(index.parent());
            do
            {
                nextCol++;
                if(nextCol == cols)
                {
                    nextRow++;
                    nextCol = 0;
                }
            }while(!editableColumns.contains(nextCol));

            if(nextRow >= rows) return QModelIndex();
            else                return index.sibling(nextRow, nextCol);
        }
    }
    else if(cursorAction == QAbstractItemView::MovePrevious)
    {
        QModelIndex index = this->currentIndex();
        if(index.isValid() && !editableColumns.isEmpty())
        {
            int nextCol = index.column();
            int nextRow = index.row();
            int cols = this->model()->columnCount();
            do
            {
                nextCol--;
                if(nextCol < 0)
                {
                    nextRow--;
                    nextCol = cols - 1;
                }
            }while(!editableColumns.contains(nextCol));

            if(nextRow < 0)     return QModelIndex();
            else                return index.sibling(nextRow, nextCol);
        }
    }
    return QTreeWidget::moveCursor(cursorAction, modifiers);
}
