#include "planhandler.h"


PlanHandler::PlanHandler(QObject *parent, Ui::Extended *ui) : QObject(parent)
{
    this->ui = ui;
    this->transparency = Opaque;
    this->inGame = false;
    this->mouseInApp = false;

    completeUI();
}


PlanHandler::~PlanHandler()
{

}


void PlanHandler::completeUI()
{
}


void PlanHandler::addMinion(QString code, bool friendly, int pos)
{
    MinionGraphicsItem* minion = new MinionGraphicsItem(code);

    if(friendly)
    {
        playerMinions.insert(pos, minion);

        int i=0;
        foreach(MinionGraphicsItem* m, playerMinions)
        {
            m->setZonePos(friendly, i, playerMinions.count());
            i++;
        }
    }
    else
    {
        enemyMinions.insert(pos, minion);

        int i=0;
        foreach(MinionGraphicsItem* m, enemyMinions)
        {
            m->setZonePos(friendly, i, enemyMinions.count());
            i++;
        }
    }

    ui->planGraphicsView->scene()->addItem(minion);
    ui->planGraphicsView->updateView(std::max(playerMinions.count(), enemyMinions.count()));
}


//void PlanHandler::removeMinion()
//{
    //recordar delete the minions
//}


void PlanHandler::updateTransparency()
{
    bool inTabPlan = ui->tabWidget->currentWidget() == ui->tabPlan;

    if(!mouseInApp && (transparency==Transparent || (inGame && transparency==AutoTransparent)))
    {
        ui->tabPlan->setAttribute(Qt::WA_NoBackground);
        ui->tabPlan->repaint();

        //Tambien nos hacemos cargo en transparency==Transparent para que se llame a MainWindowFade al empezar y terminar un juego
        if(inTabPlan && transparency!=Opaque)
        {
            emit needMainWindowFade(true);
        }
    }
    else
    {
        ui->tabPlan->setAttribute(Qt::WA_NoBackground, false);
        ui->tabPlan->repaint();

        if(inTabPlan && transparency==AutoTransparent)
        {
            emit needMainWindowFade(false);
        }
    }
}


void PlanHandler::setTransparency(Transparency value)
{
    this->transparency = value;
    updateTransparency();
}


void PlanHandler::setMouseInApp(bool value)
{
    this->mouseInApp = value;
    updateTransparency();
}
