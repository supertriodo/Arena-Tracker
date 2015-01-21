#include "logloader.h"
#include <QDebug>
#include <QThread>
#include <QFileInfo>
#include <QTimer>
#include <QFileDialog>
#include <QSettings>

LogLoader::LogLoader(QObject *parent, qint64 &logSize) : QObject(parent)
{
    fileWatcher = NULL;
    logWorker = NULL;
    gameWatcher = NULL;

    readSettings();
    logSize = getLogFileSize();
    while(logSize == 0)
    {
        qDebug() << "LogLoader: "<< "Esperando a log accesible...";
        QThread::sleep(1);
        logSize = getLogFileSize();
    }

    if(logSize > 0)
    {
        this->logSize = logSize;
        workerRunning = false;
        firstRun = true;

        logWorker = new LogWorker(this, logPath);
        connect(logWorker, SIGNAL(synchronized()),
                this, SLOT(setWorkerFinished()));
        connect(logWorker, SIGNAL(newLogLineRead(QString)),
                this, SLOT(processLogLine(QString)));
        connect(logWorker, SIGNAL(seekChanged(qint64)),
                this, SLOT(updateSeek(qint64)));

        gameWatcher = new GameWatcher(this);
        connect(gameWatcher, SIGNAL(newGameResult(GameResult)),
                this, SLOT(emitNewGameResult(GameResult)));
        connect(gameWatcher, SIGNAL(newArena(QString)),
                this, SLOT(emitNewArena(QString)));
        connect(gameWatcher, SIGNAL(newArenaReward(int,int,bool,bool,bool)),
                this, SLOT(emitNewArenaReward(int,int,bool,bool,bool)));
        connect(gameWatcher, SIGNAL(arenaRewardsComplete()),
                this, SLOT(emitArenaRewardsComplete()));



        createFileWatcher();
        prepareLogWorker(logPath);
    }
}


void LogLoader::readSettings()
{
    QString initDir;
#ifdef Q_OS_WIN32
    initDir = "c:";
#endif
#ifdef Q_OS_LINUX
    initDir = "/home";
#endif

    QSettings settings("Arena Tracker", "Arena Tracker");
    logPath = settings.value("logPath", "").toString();

    if(logPath.isEmpty() || getLogFileSize()==-1)
    {
        logPath = QFileDialog::getOpenFileName(0,
            tr("Find Hearthstone log (output_log.txt)"), initDir,
            tr("Hearthstone log (output_log.txt)"));
        settings.setValue("logPath", logPath);
    }

//   logPath = QString("/home/triodo/.Hearthstone/arena2.txt");
//   logPath = QString("/home/triodo/.PlayOnLinux/wineprefix/Hearthstone/drive_c/Program Files (x86)/Hearthstone/Hearthstone_Data/output_log.txt");

    QString logConfig = settings.value("logConfig", "").toString();

    if(logConfig.isEmpty())
    {
        logConfig = QFileDialog::getOpenFileName(0,
            tr("Find Hearthstone config log (log.config)"), initDir,
            tr("Hearthstone log (log.config)"));
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
}


void LogLoader::checkLogConfigOption(QString option, QString &data, QTextStream &stream)
{
    if(!data.contains(option))
    {
        qDebug() << "LogLoader: " << "Configurando log.config";
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
    if(fileWatcher != NULL) delete fileWatcher;
    if(gameWatcher != NULL) delete gameWatcher;
}


void LogLoader::createFileWatcher()
{
    qDebug() << "LogLoader: "<< "Creando FileWatcher.";

    if(fileWatcher != NULL) delete fileWatcher;

    fileWatcher = new QFileSystemWatcher(this);
    connect(fileWatcher, SIGNAL(fileChanged(QString)),
            this, SLOT(prepareLogWorker(QString)));

    if(fileWatcher->addPath(logPath))
    {
        qDebug() << "LogLoader: "<< "FileWatcher enlazado.";
    }
}


void LogLoader::prepareLogWorker(QString path)
{
    (void)path;
    if(workerRunning)    return;

    workerRunning = true;

    QTimer::singleShot(1000, this, SLOT(sendLogWorker()));
}


void LogLoader::sendLogWorker()
{
    qDebug() << "LogLoader: "<< "Fichero cambiado.";
    if(isLogReset())
    {
        qDebug() << "LogLoader: "<< "Worker esperando (log reiniciado).";
        workerRunning = false;
        checkFirstRun();
    }
    else
    {
        qDebug() << "LogLoader: "<< "Worker enviado.";
        logWorker->readLog();
    }
}


void LogLoader::setWorkerFinished()
{
    qDebug() << "LogLoader: "<< "Worker libre.";
    workerRunning = false;
    checkFirstRun();
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
        waitLogExists();

        logWorker->resetSeek();
        logSize = 0;

        createFileWatcher();
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
    gameWatcher->processLogLine(line);
}


//GameWatcher signals reemit
void LogLoader::emitNewGameResult(GameResult gameResult){emit newGameResult(gameResult);}
void LogLoader::emitNewArena(QString hero){emit newArena(hero);}
void LogLoader::emitNewArenaReward(int gold, int dust, bool pack, bool goldCard, bool plainCard)
{emit newArenaReward(gold, dust, pack, goldCard, plainCard);}
void LogLoader::emitArenaRewardsComplete(){emit arenaRewardsComplete();}
