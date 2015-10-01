#include "logloader.h"
#include <QtWidgets>

LogLoader::LogLoader(QObject *parent) : QObject(parent)
{
    logWorker = NULL;
}


void LogLoader::init(qint64 &logSize)
{
    readSettings();
    logSize = getLogFileSize();

    if(logSize >= 0)
    {
        emit pDebug("Log found.");
        emit pLog(tr("Log: Log found."));

        if(logSize == 0)
        {
            emit pDebug("Log is empty.");
            emit pLog(tr("Log: Log is empty."));
        }

        this->logSize = logSize;
        firstRun = true;
        updateTime = 1000;

        logWorker = new LogWorker(this, logPath);
        connect(logWorker, SIGNAL(newLogLineRead(QString, qint64)),
                this, SLOT(emitNewLogLineRead(QString, qint64)));
        connect(logWorker, SIGNAL(seekChanged(qint64)),
                this, SLOT(updateSeek(qint64)));
        connect(logWorker, SIGNAL(pLog(QString)),
                this, SIGNAL(pLog(QString)));
        connect(logWorker, SIGNAL(pDebug(QString,DebugLevel,QString)),
                this, SIGNAL(pDebug(QString,DebugLevel,QString)));
    }
    else
    {
        QSettings settings("Arena Tracker", "Arena Tracker");
        settings.setValue("logPath", "");
        emit pDebug("Log not found.");
        emit pLog(tr("Log: Log not found. Restart Arena Tracker and set the path again."));
    }
}


void LogLoader::readSettings()
{
    readLogPath();
    readLogConfigPath();
}


void LogLoader::readLogPath()
{
    QSettings settings("Arena Tracker", "Arena Tracker");
    logPath = settings.value("logPath", "").toString();

    QString logFileName;
#ifdef Q_OS_MAC
    logFileName = "Player.log";
#else
    logFileName = "output_log.txt";
#endif

    if(logPath.isEmpty() || getLogFileSize()==-1)
    {
        QMessageBox::information(0, tr("Arena Tracker"), tr("The first time you run Arena Tracker you will be asked for:\n"
                                    "1) ") + logFileName + tr(" location (If not default).\n"
                                    "2) log.config location (If not default).\n"
                                    "3) Your Arena Mastery user/password.\n"
                                    "4) Restart Hearthstone (If running).\n\n"
                                    "After your first game:\n"
                                    "5) Your Hearthstone name."));

        QString initPath = "";
#ifdef Q_OS_WIN
        initPath = "C:/Program Files (x86)/Hearthstone/Hearthstone_Data/output_log.txt";
#endif
#ifdef Q_OS_MAC
        initPath = QDir::homePath() + "/Library/Logs/Unity/Player.log";
#endif

        if(!initPath.isEmpty())
        {
            QFileInfo logFI(initPath);
            if(logFI.exists())
            {
                logPath = initPath;
            }
        }

        if(logPath.isEmpty())
        {
            logPath = QFileDialog::getOpenFileName(0,
                tr("Find Hearthstone log") + " (" + logFileName + ")", QDir::homePath(),
                "Hearthstone log (" + logFileName + ")");
        }

        settings.setValue("logPath", logPath);
    }

#ifdef QT_DEBUG
//    logPath = QString("/home/triodo/Documentos/arenaMageFull.txt");
#endif


    emit pDebug("Path "+ logFileName + ": " + logPath);
    emit pLog(tr("Settings: Path ") + logFileName + ": " + logPath);

}


void LogLoader::readLogConfigPath()
{
    QSettings settings("Arena Tracker", "Arena Tracker");
    logConfig = settings.value("logConfig", "").toString();

    if(logConfig.isEmpty())
    {
        logConfig = createDefaultLogConfig();
        if(logConfig.isEmpty())
        {
            logConfig = QFileDialog::getOpenFileName(0,
                tr("Find Hearthstone config log (log.config)"), QDir::homePath(),
                tr("log.config (log.config)"));
        }
        settings.setValue("logConfig", logConfig);
    }

    if(!logConfig.isEmpty()) checkLogConfig(logConfig);

    emit pDebug("Path log.config: " + logConfig);
    emit pLog(tr("Settings: Path log.config: ") + logConfig);
}


QString LogLoader::createDefaultLogConfig()
{
    QString initPath = "";
#ifdef Q_OS_WIN
    initPath = QDir::homePath() + "/AppData/Local/Blizzard/Hearthstone/log.config";
#endif

#ifdef Q_OS_MAC
    initPath = QDir::homePath() + "/Library/Preferences/Blizzard/Hearthstone/log.config";
#endif

#ifdef Q_OS_LINUX
#endif

    if(initPath.isEmpty()) return "";

    QFileInfo logConfigFI(initPath);
    if(logConfigFI.exists())
    {
        return initPath;
    }
    else
    {
        QString hsDir = logConfigFI.absolutePath();
        logConfigFI = QFileInfo(hsDir);
        if(logConfigFI.exists() && logConfigFI.isDir())
        {
            //Creamos log.config
            QFile logConfigFile(initPath);
            if(!logConfigFile.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                emit pDebug("Cannot create default log.config", Error);
                emit pLog(tr("Log: ERROR: Cannot create default log.config"));
                return "";
            }
            logConfigFile.close();
            return initPath;
        }
    }

    return "";
}


void LogLoader::checkLogConfig(QString logConfig)
{
    emit pDebug("Checking log.config");

    QFile file(logConfig);
    if(!file.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        emit pDebug("Cannot access log.config", Error);
        emit pLog(tr("Log: ERROR: Cannot access log.config"));
        QSettings settings("Arena Tracker", "Arena Tracker");
        settings.setValue("logConfig", "");
        return;
    }

    QString data = QString(file.readAll());
    QTextStream stream(&file);

    checkLogConfigOption("[Bob]", data, stream);
    checkLogConfigOption("[Power]", data, stream);
    checkLogConfigOption("[Rachelle]", data, stream);
    checkLogConfigOption("[Zone]", data, stream);
    checkLogConfigOption("[Ben]", data, stream);
    checkLogConfigOption("[Asset]", data, stream);

    file.close();
}


void LogLoader::checkLogConfigOption(QString option, QString &data, QTextStream &stream)
{
    if(!data.contains(option))
    {
        emit pDebug("Setting log.config");
        emit pLog(tr("Log: Setting log.config"));
        stream << endl << option << endl;
        stream << "LogLevel=1" << endl;
        stream << "ConsolePrinting=true" << endl;
//        stream << "FilePrinting=true" << endl;
//        stream << "ConsolePrinting=false" << endl;
//        stream << "ScreenPrinting=false" << endl;
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
    QFileInfo log(logPath);
    if(log.exists())
    {
        return log.size();
    }
    return -1;
}


LogLoader::~LogLoader()
{
    if(logWorker != NULL)   delete logWorker;
}


void LogLoader::sendLogWorker()
{
    if(!isLogReset())
    {
        logWorker->readLog();
    }

    workerFinished();
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
        emit pDebug("Log reset. FileSize: " + QString::number(newSize) + " < " + QString::number(logSize));
        emit pLog(tr("Log: Hearthstone started. Log reset."));
        emit seekChanged(0);
        logWorker->resetSeek();
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


QString LogLoader::getLogConfigPath()
{
    return this->logConfig;
}


//LogWorker signal reemit
void LogLoader::emitNewLogLineRead(QString line, qint64 numLine)
{
    updateTime = 500;
    emit newLogLineRead(line, numLine);
}
