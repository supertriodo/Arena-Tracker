#include "capturemanager.h"
#include "qscreen.h"
#include "../utility.h"
#include "qsettings.h"

#include <QSharedMemory>
#include <QDebug>
#include <QByteArray>
#include <QTimer>
#include <QGuiApplication>
#include <QProcessEnvironment>


bool CaptureManager::m_isWayland;
QMainWindow * CaptureManager::mainWindow;

CaptureManager& CaptureManager::instance()
{
    static CaptureManager staticInstance;
    return staticInstance;
}

CaptureManager::CaptureManager(QObject *parent)
    : QObject(parent),
    m_isCleanedUp(false),
    m_process(Q_NULLPTR),
    m_shm(Q_NULLPTR),
    m_pid(0)
{
    m_lastHeader.status = 0;
    m_shm = new QSharedMemory(QString::number(SHM_SYSV_KEY));

    connect(qApp, &QCoreApplication::aboutToQuit, this, &CaptureManager::cleanupCaptureService);
}

CaptureManager::~CaptureManager()
{
    cleanupCaptureService();
    delete m_shm;
}

void CaptureManager::init(QMainWindow *mainWindow)
{
    CaptureManager::mainWindow = mainWindow;

    QByteArray sessionType = qgetenv("XDG_SESSION_TYPE");
    qDebug() << "XDG_SESSION_TYPE:" << sessionType;
    CaptureManager::m_isWayland = (sessionType == "wayland");
}

bool CaptureManager::isWaylandSession()
{
    return CaptureManager::m_isWayland;
}

void CaptureManager::startCaptureService()
{
    if (!m_isWayland) {
        qWarning() << "Error: This solution is designed for Wayland. X11 session detected.";
        return;
    }

    m_isCleanedUp = false;

    m_process = new QProcess(this);

    connect(m_process, &QProcess::started, this, &CaptureManager::onProcessStarted);
    connect(m_process, SIGNAL(errorOccurred(QProcess::ProcessError)), this, SLOT(onProcessError(QProcess::ProcessError)));
    connect(m_process, &QProcess::readyReadStandardOutput, this, &CaptureManager::printProcessOutput);
    connect(m_process, &QProcess::readyReadStandardError, this, &CaptureManager::printProcessError);
    connect(m_process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(onProcessFinished(int,QProcess::ExitStatus)));

    QString program = Utility::extraPath() + "/captureHelper";

    qDebug() << "Starting Wayland capture service:" << program;
    m_process->start("sh", QStringList() << "-c" << "\"" + program + "\"");
}

void CaptureManager::onProcessStarted()
{
    if (!m_process) return;
    qDebug() << "Shell intermediary process started successfully. PID:" << m_process->processId();
}

void CaptureManager::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug() << "CaptureHelper process finished. Code:" << exitCode << "Status:" << exitStatus;

    if (m_process) {
        m_process->deleteLater();
        m_process = Q_NULLPTR;
    }

    m_pid = 0;
    m_lastHeader.status = 0;
    if (m_shm && m_shm->isAttached()) {
        m_shm->detach();
    }

    // Si el proceso terminó con nuestro código especial de "reinicio", lo relanzamos.
    if (exitCode == 77) {
        qDebug() << "CaptureHelper requested a restart. Relaunching...";
        startCaptureService();
    }
}

bool CaptureManager::attachToShm()
{
    if (m_shm->isAttached()) {
        return true;
    }
    if (!m_shm->attach(QSharedMemory::ReadOnly)) {
        qDebug() << "Could not attach to SHM yet:" << m_shm->errorString();
        return false;
    }
    qDebug() << "Attached to SHM segment (Key" << SHM_SYSV_KEY << ") successfully!";
    return true;
}

void CaptureManager::triggerDraftStart()
{
    qDebug() << "Capture start requested...";
    if(m_process)
    {
        qWarning() << "A capture process already exists. Not starting a new one.";
    }
    else
    {
        startCaptureService();
    }
}

void CaptureManager::triggerDraftEnd()
{
    qDebug() << "Capture end requested...";
    cleanupCaptureService();
}

void CaptureManager::cleanupCaptureService()
{
    if (m_isCleanedUp) return;
    m_isCleanedUp = true;

    qDebug() << "Cleaning up capture service...";
    if (m_shm && m_shm->isAttached()) {
        m_shm->detach();
    }

    if (m_process) {

        // Desconectamos TODOS los slots de este objeto QProcess.
        // Esto previene que se ejecuten slots como printProcessError()
        // después de que hayamos puesto m_process a Q_NULLPTR.
        m_process->disconnect();

        if (m_process->state() != QProcess::NotRunning) {
            m_process->terminate();

            if (!m_process->waitForFinished(2000)) {
                qWarning() << "captureHelper process did not respond to terminate(), forcing kill.";
                m_process->kill();
            }
            qDebug() << "captureHelper process stopped.";
        }

        m_process->deleteLater();
        m_process = Q_NULLPTR;
    }

    m_pid = 0;
    m_lastHeader.status = 0;
}

QImage CaptureManager::getLatestFrame()
{
    if (!m_shm || !m_shm->isAttached()) {
        return QImage();
    }
    m_shm->lock();
    memcpy(&m_lastHeader, m_shm->constData(), sizeof(ShmHeader));

    if (m_lastHeader.status != 1 || m_lastHeader.width <= 0 || m_lastHeader.height <= 0) {
        m_shm->unlock();
        return QImage();
    }
    const uchar* frameDataPtr = static_cast<const uchar*>(m_shm->constData()) + HEADER_OFFSET;
    QImage frame(frameDataPtr,
                 static_cast<int>(m_lastHeader.width),
                 static_cast<int>(m_lastHeader.height),
                 QImage::Format_RGB32);
    QImage copy = frame.copy();
    m_shm->unlock();
    return copy;
}


// --- Slots de Debug ---
void CaptureManager::onProcessError(QProcess::ProcessError error)
{
    qWarning() << "QProcess Error:" << error << m_process->errorString();
}

void CaptureManager::printProcessOutput()
{
    QByteArray data = m_process->readAllStandardOutput();
    if (!data.isEmpty()) {
        qDebug() << "[CaptureHelper STDOUT]:" << data.trimmed();
    }
}

void CaptureManager::printProcessError()
{
    QByteArray data = m_process->readAllStandardError();

    for (const QByteArray& line : data.split('\n')) {
        if (line.trimmed().isEmpty()) {
            continue;
        }

        // Buscamos nuestra línea especial de PID
        if (line.startsWith("PID:")) {
            QByteArray pidStr = line.mid(4);
            bool ok;
            qint64 pid = pidStr.toLongLong(&ok);
            if (ok && pid > 0) {
                if (m_pid == 0) {
                    m_pid = pid;
                    qDebug() << "Captured CaptureHelper PID:" << m_pid;

                    if (!attachToShm()) {
                        qWarning() << "First attach attempt failed. Retrying in 500ms...";
                        QTimer::singleShot(500, this, SLOT(attachToShm()));
                    }
                }
            }
        } else {
            qWarning() << "[CaptureHelper STDERR]:" << line.trimmed();
        }
    }
}

int CaptureManager::getActiveScreenIndex(bool heroDrafting) const
{
    int screenIndex = static_cast<int>(m_lastHeader.screen_index);

    if(screenIndex == -1)
    {
        QSettings settings("Arena Tracker", "Arena Tracker");
        QString tag;
        if(heroDrafting)    tag = "heroDraftingScreenIndex";
        else                tag = "draftingScreenIndex";
        screenIndex = settings.value(tag, -1).toInt();
    }

    if(screenIndex == -1)
    {
        QScreen *appScreen = QGuiApplication::screenAt(mainWindow->pos());
        if(appScreen)
        {
            screenIndex = QGuiApplication::screens().indexOf(appScreen);
        }
    }

    QList<QScreen *> screens = QGuiApplication::screens();
    if(screenIndex >= screens.count() || screenIndex < 0)
    {
        screenIndex = 0;
    }

    return screenIndex;
}













