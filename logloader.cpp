#include "logloader.h"
#include <QtWidgets>

LogLoader::LogLoader(QObject *parent) : QObject(parent)
{
    logWorker = NULL;
    gameWatcher = NULL;
}


void LogLoader::init(qint64 &logSize)
{
    readSettings();
    logSize = getLogFileSize();

    if(logSize >= 0)
    {
        qDebug() << "LogLoader: Log encontrado.";
        emit sendLog(tr("Log: Log found."));

        if(logSize == 0)
        {
            qDebug() << "LogLoader: Log vacio.";
            emit sendLog(tr("Log: Log is empty."));
        }

        this->logSize = logSize;
        firstRun = true;
        updateTime = 1000;

        logWorker = new LogWorker(this, logPath);
        connect(logWorker, SIGNAL(newLogLineRead(QString)),
                this, SLOT(processLogLine(QString)));
        connect(logWorker, SIGNAL(seekChanged(qint64)),
                this, SLOT(updateSeek(qint64)));
        connect(logWorker, SIGNAL(sendLog(QString)),
                this, SLOT(emitSendLog(QString)));

        gameWatcher = new GameWatcher(this);
        connect(gameWatcher, SIGNAL(newGameResult(GameResult)),
                this, SLOT(emitNewGameResult(GameResult)));
        connect(gameWatcher, SIGNAL(newArena(QString)),
                this, SLOT(emitNewArena(QString)));
        connect(gameWatcher, SIGNAL(newArenaReward(int,int,bool,bool,bool)),
                this, SLOT(emitNewArenaReward(int,int,bool,bool,bool)));
        connect(gameWatcher, SIGNAL(arenaRewardsComplete()),
                this, SLOT(emitArenaRewardsComplete()));
        connect(gameWatcher, SIGNAL(newDeckCard(QString)),
                this, SLOT(emitNewDeckCard(QString)));
        connect(gameWatcher, SIGNAL(sendLog(QString)),
                this, SLOT(emitSendLog(QString)));
        connect(gameWatcher, SIGNAL(startGame()),
                this, SLOT(emitStartGame()));
        connect(gameWatcher, SIGNAL(endGame()),
                this, SLOT(emitEndGame()));
        connect(gameWatcher, SIGNAL(cardDrawn(QString)),
                this, SLOT(emitCardDrawn(QString)));

        QTimer::singleShot(updateTime, this, SLOT(sendLogWorker()));
    }
    else
    {
        QSettings settings("Arena Tracker", "Arena Tracker");
        settings.setValue("logPath", "");
        qDebug() << "LogLoader: Log no encontrado.";
        emit sendLog(tr("Log: Log not found. Restart Arena Tracker and set the path again."));
    }
}


void LogLoader::readSettings()
{
    QSettings settings("Arena Tracker", "Arena Tracker");
    logPath = settings.value("logPath", "").toString();

    if(logPath.isEmpty() || getLogFileSize()==-1)
    {
        QMessageBox::information(0, tr("Arena Tracker"), tr("The first time you run Arena Tracker you will be asked for:\n"
                                    "1) output_log.txt location.\n"
                                    "2) log.config location.\n"
                                    "3) Your Arena Mastery user/password.\n\n"
                                    "After your first game:\n"
                                    "4) Your Hearthstone name."));

        QString initDir;
#ifdef Q_OS_WIN32
        initDir = QDir::toNativeSeparators("C:/Program Files (x86)/Hearthstone/Hearthstone_Data/output_log.txt");
#endif
#ifdef Q_OS_LINUX
        initDir = QDir::homePath();
#endif
        logPath = QFileDialog::getOpenFileName(0,
            tr("Find Hearthstone log (output_log.txt)"), initDir,
            tr("Hearthstone log (output_log.txt)"));
        settings.setValue("logPath", logPath);
    }

#ifdef QT_DEBUG
//    logPath = QString("/home/triodo/Documentos/arenaMagoFull.txt");
#endif

    QString logConfig = settings.value("logConfig", "").toString();

    if(logConfig.isEmpty())
    {
        QString initDir;
#ifdef Q_OS_WIN32
        //initDir = QDir::toNativeSeparators(QDir::homePath() + "/Local Settings/Application Data/Blizzard/Hearthstone/log.config");
        initDir = QDir::toNativeSeparators(QDir::homePath() + "/AppData/Local/Blizzard/Hearthstone/log.config");
#endif
#ifdef Q_OS_LINUX
        initDir = QDir::homePath();
#endif
        logConfig = QFileDialog::getOpenFileName(0,
            tr("Find Hearthstone config log (log.config)"), initDir,
            tr("(*.*)"));
        settings.setValue("logConfig", logConfig);
    }
    if(!logConfig.isNull()) checkLogConfig(logConfig);

    qDebug() << "LogLoader: " << "Path output_log.txt " << logPath;
    qDebug() << "LogLoader: " << "Path log.config " << logConfig;
}


void LogLoader::checkLogConfig(QString logConfig)
{
    qDebug() << "LogLoader: " << "Verificando log.config";

    QFile *file = new QFile(logConfig);
    if(!file->open(QIODevice::ReadWrite | QIODevice::Text))
    {
        qDebug() << "LogLoader: "<< "ERROR: No se puede acceder a log.config.";
        emit sendLog(tr("Log: ERROR:Cannot access log.config"));
        QSettings settings("Arena Tracker", "Arena Tracker");
        settings.setValue("logConfig", "");
        return;
    }

    QString data = QString(file->readAll());
    QTextStream stream(file);

    checkLogConfigOption("[Bob]", data, stream);
    checkLogConfigOption("[Power]", data, stream);
    checkLogConfigOption("[Rachelle]", data, stream);
    checkLogConfigOption("[Zone]", data, stream);
    checkLogConfigOption("[Ben]", data, stream);
    checkLogConfigOption("[Asset]", data, stream);

    file->close();
}


void LogLoader::checkLogConfigOption(QString option, QString &data, QTextStream &stream)
{
    if(!data.contains(option))
    {
        qDebug() << "LogLoader: " << "Configurando log.config";
        emit sendLog(tr("Log: Setting log.config"));
        stream << endl << option << endl;
        stream << "LogLevel=1" << endl;
        stream << "ConsolePrinting=true" << endl;
    }
}


/*
 * Return
 * -1 no existe
 * 0 inaccesible
 * n size
 */
qint64 LogLoader::getLogFileSize()
{
    QFileInfo *log = new QFileInfo(logPath);
    if(log->exists())
    {
        return log->size();
    }
    return -1;
}



void LogLoader::waitLogExists()
{
    QFileInfo *log = new QFileInfo(logPath);

    while(!log->exists())
    {
        qDebug() << "LogLoader: "<< "Esperando a log creado...";
        QThread::sleep(1);
    }
}


LogLoader::~LogLoader()
{
    if(logWorker != NULL)   delete logWorker;
    if(gameWatcher != NULL) delete gameWatcher;
}


void LogLoader::sendLogWorker()
{
    if(isLogReset())
    {
        waitLogExists();
    }

//    qDebug() << "LogLoader: "<< "Worker enviado: " << updateTime;
    logWorker->readLog();
    workerFinished();
//    qDebug() << "LogLoader: "<< "Worker libre.";
}


void LogLoader::workerFinished()
{
    checkFirstRun();
    QTimer::singleShot(updateTime, this, SLOT(sendLogWorker()));

    if(updateTime < 4000) updateTime += 500;
}


void LogLoader::checkFirstRun()
{
    if(firstRun)
    {
        firstRun = false;
        emit synchronized();
    }
}


bool LogLoader::isLogReset()
{
    qint64 newSize = getLogFileSize();

    if(newSize == 0)    return false;

    if((newSize == -1) || (newSize < logSize))
    {
        //Log se ha reiniciado
        qDebug() << "LogLoader: "<< "Log reiniciado. FileSize: " << newSize << " < " << logSize;
        emit sendLog(tr("Log: Hearthstone started. Log reset."));
        emit seekChanged(0);
        logWorker->resetSeek();
        gameWatcher->reset();
        logSize = 0;
        return true;
    }
    else
    {
        logSize = newSize;
        return false;
    }
}


void LogLoader::updateSeek(qint64 logSeek)
{
    if(firstRun)    emit seekChanged(logSeek);
}


void LogLoader::processLogLine(QString line)
{
    updateTime = 500;
    gameWatcher->processLogLine(line);
}


void LogLoader::setDeckRead()
{
    gameWatcher->setDeckRead();
}


//GameWatcher signals reemit
void LogLoader::emitNewGameResult(GameResult gameResult){emit newGameResult(gameResult);}
void LogLoader::emitNewArena(QString hero){emit newArena(hero);}
void LogLoader::emitNewArenaReward(int gold, int dust, bool pack, bool goldCard, bool plainCard)
{emit newArenaReward(gold, dust, pack, goldCard, plainCard);}
void LogLoader::emitArenaRewardsComplete(){emit arenaRewardsComplete();}
void LogLoader::emitNewDeckCard(QString card){emit newDeckCard(card);}
void LogLoader::emitSendLog(QString line){emit sendLog(line);}
void LogLoader::emitStartGame(){emit startGame();}
void LogLoader::emitEndGame(){emit endGame();}
void LogLoader::emitCardDrawn(QString code){emit cardDrawn(code);}




