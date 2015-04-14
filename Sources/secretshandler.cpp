#include "secretshandler.h"
#include "secretcard.h"

SecretsHandler::SecretsHandler(QObject *parent, Ui::MainWindow *ui) : QObject(parent)
{
    this->ui = ui;

    completeUI();

    //test
//    SecretCard secretCard("");
//    secretCard.treeItem = ui->secretsTreeWidget
}

SecretsHandler::~SecretsHandler()
{

}


void SecretsHandler::completeUI()
{
    ui->secretsTreeWidget->setHidden(true);

    ui->secretsTreeWidget->setColumnCount(1);
    ui->secretsTreeWidget->header()->close();
    ui->secretsTreeWidget->setIconSize(CARD_SIZE);
}
