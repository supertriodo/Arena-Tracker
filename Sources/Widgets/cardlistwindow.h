#ifndef SECRETSWINDOW_H
#define SECRETSWINDOW_H

#include <QMainWindow>
#include <QObject>
#include "movelistwidget.h"
#include "../secretshandler.h"
#include "../Cards/synergycard.h"


class CardListWindow : public QMainWindow
{
    Q_OBJECT

//Constructor
public:
    CardListWindow(QWidget *parent, SecretsHandler *secretsHandler);
    ~CardListWindow() Q_DECL_OVERRIDE;

//Variables
private:
    MoveListWidget * listWidget;
    SecretsHandler * secretsHandler;

//Metodos
protected:
    void leaveEvent(QEvent *e) Q_DECL_OVERRIDE;
    void enterEvent(QEvent *e) Q_DECL_OVERRIDE;

signals:

public slots:
    void loadSecret(int id, QRect &rectCard, int maxTop=-1, int maxBottom=-1, bool alignReverse=false);
    void loadDraftItem(QList<SynergyCard> &synergyCardList, QRect &rectCard, int maxTop=-1, int maxBottom=-1, bool alignReverse=false);
    void loadDraftOverlayItem(QList<SynergyCard> &synergyCardList, QPoint &originList, int maxLeft=-1, int maxRight=-1);
};


#endif // SECRETSWINDOW_H
