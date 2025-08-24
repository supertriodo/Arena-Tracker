#ifndef MOVELISTWIDGET_H
#define MOVELISTWIDGET_H

#include <QObject>
#include <QListWidget>
#include <QStyledItemDelegate>


class PixelPerfectDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit PixelPerfectDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};



class MoveListWidget : public QListWidget
{
    Q_OBJECT

public:
    MoveListWidget(QWidget *parent = nullptr);

private:
    PixelPerfectDelegate* m_delegate;

protected:
    void mouseDoubleClickEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *e) Q_DECL_OVERRIDE;

public:
    void setTheme();

signals:
    void xLeave();
    void leave();
};

#endif // MOVELISTWIDGET_H
