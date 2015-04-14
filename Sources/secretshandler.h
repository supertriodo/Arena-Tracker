#ifndef SECRETSHANDLER_H
#define SECRETSHANDLER_H

#include "ui_mainwindow.h"
#include "deckcard.h"
#include <QObject>


//Secretos
#define MIRROR_ENTITY QString("EX1_294")


class ActiveSecret
{

};


class SecretsHandler : public QObject
{
    Q_OBJECT
public:
    SecretsHandler(QObject *parent, Ui::MainWindow *ui);
    ~SecretsHandler();

//Variables
private:
    Ui::MainWindow *ui;

//Metodos
private:
    void completeUI();

signals:

public slots:
};

#endif // SECRETSHANDLER_H
