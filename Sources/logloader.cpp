#include "logloader.h"
#include <QtWidgets>

LogLoader::LogLoader(QObject *parent) : QObject(parent)
{
    logWorker = NULL;
    setUpdateTimeMax();
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

        QTimer::singleShot(1000, this, SLOT(sendLogWorker())); //Retraso para dejar que la aplicacion se pinte.
    }
    else
    {
        QSettings settings("Arena Tracker", "Arena Tracker");
        settings.setValue("logPath", "");
        emit pDebug("Log not found.");
        emit pLog(tr("Log: Log not found. Restart Arena Tracker and set the path again."));
        QMessageBox::information((QMainWindow*)this->parent(), tr("Log not found"), tr("Log not found. Restart Arena Tracker and set the path again."));
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
                                    "3) Restart Hearthstone (If running).\n\n"
                                    "After your first game:\n"
                                    "5) Your Hearthstone name."));

        QString initPath = "";
        logPath = "";
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
//    logPath = QString("/home/triodo/Documentos/test.txt");
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
        if(!logConfig.isEmpty())
        {
            //Remove old log.config
            QFile file(logConfig);
            if(file.exists())   file.remove();

            checkLogConfig();
            QMessageBox::information(0, tr("Restart Hearthstone"), tr("Restart Hearthstone (If running)."));
        }
    }
    else
    {
        checkLogConfig();
    }

    emit pDebug("Path log.config: " + logConfig);
    emit pLog(tr("Settings: Path log.config: ") + logConfig);
    emit logConfigSet();
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
            return initPath;
        }
    }

    return "";
}


void LogLoader::checkLogConfig()
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

    checkLogConfigOption("[LoadingScreen]", data, stream);
    checkLogConfigOption("[Power]", data, stream);
    checkLogConfigOption("[Zone]", data, stream);
    checkLogConfigOption("[Arena]", data, stream);

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
        if(option == "[Power]") stream << "Verbose=1" << endl;
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
    if(updateTime < maxUpdateTime)  updateTime += UPDATE_TIME_STEP;
}


void LogLoader::setUpdateTimeMin()
{
    setMaxUpdateTime(MIN_UPDATE_TIME);
}


void LogLoader::setUpdateTimeMax()
{
    setMaxUpdateTime(MAX_UPDATE_TIME);
}


void LogLoader::setMaxUpdateTime(int value)
{
    maxUpdateTime = value;
    updateTime = std::min(updateTime,maxUpdateTime);
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
    updateTime = std::min(MIN_UPDATE_TIME,maxUpdateTime);
    emit newLogLineRead(line, numLine);
}
