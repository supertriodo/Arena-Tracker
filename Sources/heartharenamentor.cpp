#include "heartharenamentor.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QtWidgets>

HearthArenaMentor::HearthArenaMentor(QObject *parent) : QObject(parent)
{
    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));
}

HearthArenaMentor::~HearthArenaMentor()
{

}


void HearthArenaMentor::askCardsRating(QString &hero, QList<int> &draftedCards, int cards[3])
{
    //http://draft.heartharena.com/arena/option-multi-score/7/-/716-554-52
    //http://draft.heartharena.com/arena/option-multi-score/7/52/253-255-769
    //http://draft.heartharena.com/arena/option-multi-score/7/52-769/253-167-251

    QString url = "http://draft.heartharena.com/arena/option-multi-score/";
    url += heroToWebNumber(hero) + "/";

    if(draftedCards.isEmpty())  url += "-/";
    else
    {
        for(int i=0; i<(draftedCards.size()-1); i++)
        {
            url += QString::number(draftedCards[i]) + "-";
        }
        url += QString::number(draftedCards.last()) + "/";
    }

    url += QString::number(cards[0]) + "-" + QString::number(cards[1]) + "-" + QString::number(cards[2]);

//    qDebug() << "HearthArenaMentor: Asked to HearthArena. Url:" << url;

    networkManager->get(QNetworkRequest(QUrl(url)));
}


void HearthArenaMentor::replyFinished(QNetworkReply *reply)
{
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError)
    {
        qDebug() << "HearthArenaMentor: " << "ERROR: accediendo a Hearth Arena.";
        emit sendLog(tr("Web: No internet access to Hearth Arena."));
        return;
    }

    //{"tip":{"text":"Let\u0027s go purely by the tier list: \u003Cb\u003EMad Bomber\u003C\/b\u003E.","face":"60-70"},
    //"results":[
    //{"card":{"id":374,"name":"Mad Bomber","image":"EX1_082","explain":null,"score":65.95,"tierScore":66,
    //    "synergies":["Al\u0027Akir the Windlord","Annoy-o-Tron"],"mechanics":{"0":"twoDrop","1":"ping","2":"reach"}},"deck":{"synergies":[]}},
    //{"card":{"id":334,"name":"Frostwolf Grunt","image":"CS2_121","explain":null,"score":31.95,"tierScore":32,
    //    "synergies":[],"mechanics":{"0":"twoDrop","1":"survivability"}},"deck":{"synergies":[]}},
    //{"card":{"id":759,"name":"Ship\u0027s Cannon","image":"GVG_075","explain":null,"score":50.02,"tierScore":50,
    //    "synergies":[],"mechanics":{"0":"twoDrop"}},"deck":{"synergies":[]}}
    //          ]}

    QString tip;
    double ratings[3];
    QString synergies[3];

    QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll());
    QJsonObject jsonObject = jsonDoc.object();
    tip = jsonObject.value("tip").toObject().value("text").toString();

    for(int i=0; i<3; i++)
    {
        QJsonObject jsonCard = jsonObject.value("results").toArray().at(i).toObject().value("card").toObject();
        ratings[i] = jsonCard.value("score").toDouble();
        QJsonArray jsonSynergies = jsonCard.value("synergies").toArray();

        for(QJsonArray::const_iterator it=jsonSynergies.constBegin(); it!=jsonSynergies.constEnd(); it++)
        {
            synergies[i] += (*it).toString() + ". ";
        }
    }

    qDebug() << "HearthArenaMentor: Tip:" << tip;
//    qDebug() << "HearthArenaMentor: Ratings:" << ratings[0] << ratings[1] << ratings[2];
//    qDebug() << "HearthArenaMentor: Synergies:" << synergies[0] << synergies[1] << synergies[2];
    emit newTip(tip,ratings[0],ratings[1],ratings[2],synergies[0],synergies[1],synergies[2]);
}


QString HearthArenaMentor::heroToWebNumber(const QString &hero)
{
    if(hero.compare("06")==0)           return QString("1");
    else if(hero.compare("05")==0)      return QString("2");
    else if(hero.compare("08")==0)      return QString("3");
    else if(hero.compare("04")==0)      return QString("4");
    else if(hero.compare("09")==0)      return QString("5");
    else if(hero.compare("03")==0)      return QString("6");
    else if(hero.compare("02")==0)      return QString("7");
    else if(hero.compare("07")==0)      return QString("8");
    else if(hero.compare("01")==0)      return QString("9");
    else return QString();
}
