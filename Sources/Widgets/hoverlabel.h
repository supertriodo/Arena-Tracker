#ifndef HOVERLABEL_H
#define HOVERLABEL_H

#include <QObject>
#include <QLabel>

class HoverLabel : public QLabel
{
    Q_OBJECT

public:
    HoverLabel(QWidget *parent = nullptr);

protected:
    bool event(QEvent *e) Q_DECL_OVERRIDE;

signals:
    void enter(HoverLabel *hoverLabel);
    void leave(HoverLabel *hoverLabel);
    void click(HoverLabel *hoverLabel);
};

#endif // HOVERLABEL_H
