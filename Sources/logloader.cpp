#include "logloader.h"
#include <QtWidgets>

LogLoader::LogLoader(QObject *parent) : QObject(parent)
{
    updateTime = MIN_UPDATE_TIME;
    setUpdateTimeMax();

    logComponentList.append("LoadingScreen");
    logComponentList.append("Power");
    logComponentList.append("Zone");
    logComponentList.append("Arena");
    logComponentList.append("Asset");

    match = new QRegularExpressionMatch();
}


bool LogLoader::init()
{
    if(!readSettings()) return false;

    emit pDebug("Log found.");

    updateTime = 1000;

    createLogWorkers();

    QTimer::singleShot(1, this, SLOT(sendLogWorkerFirstRun())); //Retraso para dejar que la aplicacion se pinte.
    return true;
}


void LogLoader::createLogWorkers()
{
    for(const QString &component: qAsConst(logComponentList))
    {
        createLogWorker(component);
    }
}


void LogLoader::createLogWorker(QString logComponent)
{
    LogWorker *logWorker;
    logWorker = new LogWorker(this, logsDirPath, logComponent);
    connect(logWorker, SIGNAL(pDebug(QString,DebugLevel,QString)),
            this, SIGNAL(pDebug(QString,DebugLevel,QString)));

    if(logComponent == "LoadingScreen")
    {
        connect(logWorker, SIGNAL(logReset()),
                this, SIGNAL(logReset()));
        connect(logWorker, SIGNAL(newLogLineRead(LogComponent,QString,qint64,qint64)),
                this, SLOT(emitNewLogLineRead(LogComponent,QString,qint64,qint64)));
    }

    logWorkerMap[logComponent] = logWorker;
}


bool LogLoader::readSettings()
{
    sortLogs = true;
    return readLogsDirPath() && readLogConfigPath();
}


QString LogLoader::findLinuxLogs(QString pattern)
{
    QProcess p;
    p.start("find \"" + QDir::homePath() + "\" -wholename \"" + pattern + "\"");
    p.waitForFinished(-1);
    QString path = QString(p.readAll()).trimmed();
    emit pDebug(pattern + " " + QString(path.isEmpty()?"missing":"found") + " on disk.");
    return path;
}


bool LogLoader::readLogsDirPath()
{
    QSettings settings("Arena Tracker", "Arena Tracker");
    logsDirPath = settings.value("logsDirPath", "").toString();

    if(logsDirPath.isEmpty())
    {
        QString initPath = "";
        logsDirPath = "";
#ifdef Q_OS_WIN
        initPath = "C:/Program Files (x86)/Hearthstone";
#endif
#ifdef Q_OS_MAC
        initPath = "/Applications/Hearthstone";
#endif
#ifdef Q_OS_LINUX
        initPath = findLinuxLogs("*/Program Files*/Hearthstone");
#endif

        if(!initPath.isEmpty())
        {
            if(QFileInfo::exists(initPath))
            {
                logsDirPath = initPath + "/Logs";
                if(!QFileInfo::exists(logsDirPath))
                {
                    QDir().mkdir(logsDirPath);
                    emit pDebug(logsDirPath + " created.");
                }
            }
        }

        if(logsDirPath.isEmpty())
        {
            emit pDebug("Show Find Logs dir dialog.");
            logsDirPath = QFileDialog::getExistingDirectory(nullptr,
                "Find Hearthstone Logs dir",
                QDir::homePath());
        }

        settings.setValue("logsDirPath", logsDirPath);
    }

    emit pDebug("Path Logs Dir: " + logsDirPath + " - " + QString::number(logsDirPath.length()));

    if(!QFileInfo::exists(logsDirPath))
    {
        settings.setValue("logsDirPath", "");
        emit pDebug("Logs dir not found.");
        QMessageBox::information(static_cast<QWidget*>(this->parent()), tr("Logs dir not found"), tr("Logs dir not found. Restart Arena Tracker and set the path again."));
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
            emit pDebug("Show Find log.config dialog.");
            logConfig = QFileDialog::getOpenFileName(nullptr,
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

    emit pDebug("Path log.config: " + logConfig + " - " + QString::number(logConfig.length()));

    if(!QFileInfo::exists(logConfig))
    {
        settings.setValue("logConfig", "");
        emit pDebug("log.config not found.");
        QMessageBox::information(static_cast<QWidget*>(this->parent()), tr("log.config not found"), tr("log.config not found. Restart Arena Tracker and set the path again."));
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
#ifdef Q_OS_LINUX
    initPath = findLinuxLogs("*/AppData/Local/Blizzard/Hearthstone");
    if(initPath.isEmpty())      initPath = findLinuxLogs("*/Local Settings/Application Data/Blizzard/Hearthstone");
    if(!initPath.isEmpty())     initPath += "/log.config";
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
        emit pDebug("Cannot access log.config", DebugLevel::Error);
        QSettings settings("Arena Tracker", "Arena Tracker");
        settings.setValue("logConfig", "");
        QMessageBox::information(static_cast<QWidget*>(this->parent()), tr("log.config not found"), tr("log.config not found. Restart Arena Tracker and set the path again."));
        return false;
    }

    QString data = QString(file.readAll());
    QTextStream stream(&file);

    bool logConfigChanged = false;
    for(const QString &component: qAsConst(logComponentList))
    {
        logConfigChanged = checkLogConfigOption("["+component+"]", data, stream) || logConfigChanged;
    }

    file.close();

    if(logConfigChanged)
    {
        emit showMessageProgressBar("Restart Hearthstone");
    }

    return true;
}


bool LogLoader::checkLogConfigOption(QString option, QString &data, QTextStream &stream)
{
    if(!data.contains(option))
    {
        emit pDebug("Setting log.config");
        stream << endl << option << endl;
        stream << "LogLevel=1" << endl;
        stream << "FilePrinting=true" << endl;
        if(option == "[Power]") stream << "Verbose=1" << endl;

        return true;
    }
    else    return false;
}


LogLoader::~LogLoader()
{
    const QList<LogWorker *> workerList = logWorkerMap.values();
    for(const LogWorker *worker: workerList)   delete worker;
    logWorkerMap.clear();
    delete match;
}


void LogLoader::sendLogWorkerFirstRun()
{
    for(const QString &logComponent: qAsConst(logComponentList))
    {
        LogWorker *logWorker = logWorkerMap[logComponent];
        logWorker->readLog();

        if(logComponent != "LoadingScreen")
        {
            connect(logWorker, SIGNAL(newLogLineRead(LogComponent,QString,qint64,qint64)),
                    this, SLOT(emitNewLogLineRead(LogComponent,QString,qint64,qint64)));
        }
    }

    QTimer::singleShot(updateTime, this, SLOT(sendLogWorker()));
    emit synchronized();
}


void LogLoader::sendLogWorker()
{
    for(const QString &logComponent: qAsConst(logComponentList))    logWorkerMap[logComponent]->readLog();
    processDataLogs();

    QTimer::singleShot(updateTime, this, SLOT(sendLogWorker()));
    if(updateTime < maxUpdateTime)  updateTime += UPDATE_TIME_STEP;
}


void LogLoader::processDataLogs()
{
    if(dataLogs.isEmpty())  return;

    QList<qint64> timeStamps = dataLogs.keys();
    qSort(timeStamps);

    for(qint64 timeStamp: qAsConst(timeStamps))
    {
        DataLog dataLog = dataLogs[timeStamp];
        emit newLogLineRead(dataLog.logComponent, dataLog.line, dataLog.numLine, dataLog.logSeek);
    }
    dataLogs.clear();
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


QString LogLoader::getLogConfigPath()
{
    return this->logConfig;
}


QString LogLoader::getLogsDirPath()
{
    return this->logsDirPath;
}


void LogLoader::addToDataLogs(LogComponent logComponent, QString line, qint64 numLine, qint64 logSeek)
{
    if(line.contains(QRegularExpression("(\\d+):(\\d+):(\\d+).(\\d+) (.*)"), match))
    {
        DataLog dataLog;
        dataLog.logComponent = logComponent;
        dataLog.line = match->captured(5);
        dataLog.numLine = numLine;
        dataLog.logSeek = logSeek;

        qint64 timeStamp = QString(match->captured(1) + match->captured(2) + match->captured(3) + match->captured(4)).toLongLong();
        while(dataLogs.contains(timeStamp))     timeStamp++;
        dataLogs[timeStamp] = dataLog;
    }
    else
    {
        emit pDebug("Log timestamp invalid: " + line, DebugLevel::Error);
        emit newLogLineRead(logComponent, line, numLine, logSeek);
    }
}


//LogWorker signal reemit
void LogLoader::emitNewLogLineRead(LogComponent logComponent, QString line, qint64 numLine, qint64 logSeek)
{
    updateTime = MIN_UPDATE_TIME;
    if(sortLogs)    addToDataLogs(logComponent, line, numLine, logSeek);
    else            emit newLogLineRead(logComponent, line, numLine, logSeek);
}


