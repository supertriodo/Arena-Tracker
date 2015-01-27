#include "logloader.h"
#include <QDebug>
#include <QThread>
#include <QFileInfo>
#include <QTimer>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>

LogLoader::LogLoader(QObject *parent) : QObject(parent)
{
    fileWatcher = NULL;
    logWorker = NULL;
    gameWatcher = NULL;
}


void LogLoader::init(qint64 &logSize)
{
    readSettings();
    logSize = getLogFileSize();
    while(logSize == 0)
    {
        qDebug() << "LogLoader: "<< "Esperando a log accesible...";
        emit sendLog(tr("Log: WARNING:Cannot access log..."));
        QThread::sleep(1);
        logSize = getLogFileSize();
    }

    if(logSize > 0)
    {
        emit sendLog(tr("Log: Linked to log."));
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
        connect(gameWatcher, SIGNAL(sendLog(QString)),
                this, SLOT(emitSendLog(QString)));

        createFileWatcher();
        prepareLogWorker(logPath);
    }
    else
    {
        emit sendLog(tr("Log: Log not found."));
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
//   logPath = QString("/home/triodo/arena2.txt");
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
        emit sendLog(tr("Log: Hearthstone started. Log reset."));
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
void LogLoader::emitSendLog(QString line){emit sendLog(line);}
