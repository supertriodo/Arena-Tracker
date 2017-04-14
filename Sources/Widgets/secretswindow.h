#ifndef SECRETSWINDOW_H
#define SECRETSWINDOW_H

#include <QMainWindow>
#include <QObject>
#include "movelistwidget.h"
#include "../secretshandler.h"


class SecretsWindow : public QMainWindow
{
    Q_OBJECT

//Constructor
public:
    SecretsWindow(QWidget *parent, SecretsHandler *secretsHandler);
    ~SecretsWindow();

//Variables
private:
    MoveListWidget * listWidget;
    SecretsHandler * secretsHandler;

//Metodos
public:


signals:

public slots:
    void loadSecret(int id, QRect rectCard, int maxTop=-1, int maxBottom=-1, bool alignReverse=false);
};


#endif // SECRETSWINDOW_H
