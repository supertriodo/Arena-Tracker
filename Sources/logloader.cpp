#include "logloader.h"
#include <QtWidgets>

LogLoader::LogLoader(QObject *parent) : QObject(parent)
{
    setUpdateTimeMax();
    logComponentList.append("LoadingScreen");
    logComponentList.append("Power");
    logComponentList.append("Zone");
    logComponentList.append("Arena");
}


bool LogLoader::init()
{
    if(!readSettings()) return false;

    emit pDebug("Log found.");
    emit pLog(tr("Log: Log found."));

    updateTime = 1000;

    createLogWorkers();

    QTimer::singleShot(1000, this, SLOT(sendLogWorker())); //Retraso para dejar que la aplicacion se pinte.
    return true;
}


void LogLoader::createLogWorkers()
{
    foreach(QString component, logComponentList)
    {
        createLogWorker(component);
    }
}


void LogLoader::createLogWorker(QString logComponent)
{
    LogWorker *logWorker;
    logWorker = new LogWorker(this, logsDirPath, logComponent);
    connect(logWorker, SIGNAL(newLogLineRead(LogComponent, QString, qint64, qint64)),
            this, SLOT(emitNewLogLineRead(LogComponent, QString, qint64, qint64)));
    connect(logWorker, SIGNAL(pLog(QString)),
            this, SIGNAL(pLog(QString)));
    connect(logWorker, SIGNAL(pDebug(QString,DebugLevel,QString)),
            this, SIGNAL(pDebug(QString,DebugLevel,QString)));

    if(logComponent == "LoadingScreen")
    {
        connect(logWorker, SIGNAL(logReset()),
                this, SIGNAL(logReset()));
    }

    logWorkerList.append(logWorker);
}


bool LogLoader::readSettings()
{
    return readLogsDirPath() && readLogConfigPath();
}


bool LogLoader::readLogsDirPath()
{
    QSettings settings("Arena Tracker", "Arena Tracker");
    logsDirPath = settings.value("logsDirPath", "").toString();

    if(logsDirPath.isEmpty())
    {
        QMessageBox::information(0, "Arena Tracker", "The first time you run Arena Tracker you will be asked for:\n"
                                    "1) Logs dir location (If not default).\n"
                                    "2) log.config location (If not default).\n"
                                    "3) Start Hearthstone (Restart if running)."
                                    );

        QString initPath = "";
        logsDirPath = "";
#ifdef Q_OS_WIN
        initPath = "C:/Program Files (x86)/Hearthstone/Logs";
#endif
#ifdef Q_OS_MAC
        initPath = "/Applications/Hearthstone/Logs";
#endif
#ifdef Q_OS_ANDROID
    initPath = "/sdcard/Android/data/com.blizzard.wtcg.hearthstone/files/Logs";
#endif

        if(!initPath.isEmpty())
        {
            QFileInfo logFI(initPath);
            if(logFI.exists())
            {
                logsDirPath = initPath;
            }
        }

        if(logsDirPath.isEmpty())
        {
            logsDirPath = QFileDialog::getExistingDirectory(0,
                "Find Hearthstone Logs dir.",
                QDir::homePath());
        }

        settings.setValue("logsDirPath", logsDirPath);
    }

    emit pDebug("Path Logs Dir: " + logsDirPath);
    emit pLog("Settings: Path Logs Dir: " + logsDirPath);

    if(!QFileInfo(logsDirPath).exists())
    {
        settings.setValue("logsDirPath", "");
        emit pDebug("Logs dir not found.");
        emit pLog(tr("Log: Logs dir not found. Restart Arena Tracker and set the path again."));
        QMessageBox::information(0, tr("Logs dir not found"), tr("Logs dir not found. Restart Arena Tracker and set the path again."));
        return false;
    }
    return true;
}


bool LogLoader::readLogConfigPath()
{
    bool isOk = true;
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

            isOk = checkLogConfig();
        }
    }
    else
    {
        isOk = checkLogConfig();
    }

    emit pDebug("Path log.config: " + logConfig);
    emit pLog(tr("Settings: Path log.config: ") + logConfig);

    if(!QFileInfo(logConfig).exists())
    {
        settings.setValue("logConfig", "");
        emit pDebug("log.config not found.");
        emit pLog(tr("Log: log.config not found. Restart Arena Tracker and set the path again."));
        QMessageBox::information(0, tr("log.config not found"), tr("log.config not found. Restart Arena Tracker and set the path again."));
        return false;
    }

    if(isOk)    emit logConfigSet();
    return isOk;
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
#ifdef Q_OS_ANDROID
    initPath = "/sdcard/Android/data/com.blizzard.wtcg.hearthstone/files/log.config";
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


bool LogLoader::checkLogConfig()
{
    emit pDebug("Checking log.config");

    QFile file(logConfig);
    if(!file.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        emit pDebug("Cannot access log.config", Error);
        emit pLog(tr("Log: ERROR: Cannot access log.config"));
        QSettings settings("Arena Tracker", "Arena Tracker");
        settings.setValue("logConfig", "");
        QMessageBox::information(0, tr("log.config not found"), tr("log.config not found. Restart Arena Tracker and set the path again."));
        return false;
    }

    QString data = QString(file.readAll());
    QTextStream stream(&file);

    bool logConfigChanged = false;
    foreach(QString component, logComponentList)
    {
        logConfigChanged = checkLogConfigOption("["+component+"]", data, stream) || logConfigChanged;
    }

    file.close();

    if(logConfigChanged)
    {
        QMessageBox::information(0, tr("Restart Hearthstone"), tr("log.config has been modified.\nStart Hearthstone (Restart if running)."));
    }

    return true;
}


bool LogLoader::checkLogConfigOption(QString option, QString &data, QTextStream &stream)
{
    if(!data.contains(option))
    {
        emit pDebug("Setting log.config");
        emit pLog(tr("Log: Setting log.config"));
        stream << endl << option << endl;
        stream << "LogLevel=1" << endl;
        stream << "ConsolePrinting=true" << endl;
        stream << "FilePrinting=true" << endl;
        if(option == "[Power]") stream << "Verbose=1" << endl;

        return true;
    }
    else    return false;
}


LogLoader::~LogLoader()
{
    foreach(LogWorker *worker, logWorkerList)   delete worker;
    logWorkerList.clear();
}


void LogLoader::sendLogWorker()
{
    foreach(LogWorker *worker, logWorkerList)   worker->readLog();

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


void LogLoader::copyGameLog(qint64 logSeekCreate, qint64 logSeekWon, QString fileName)
{
//    if(logWorker == NULL)   return;
//    logWorker->copyGameLog(logSeekCreate, logSeekWon, fileName);//TODO
}


QString LogLoader::getLogConfigPath()
{
    return this->logConfig;
}


//LogWorker signal reemit
void LogLoader::emitNewLogLineRead(LogComponent logComponent, QString line, qint64 numLine, qint64 logSeek)
{
    updateTime = std::min(MIN_UPDATE_TIME,maxUpdateTime);
    emit newLogLineRead(logComponent, line, numLine, logSeek);
}


