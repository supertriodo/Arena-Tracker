#ifndef CAPTUREMANAGER_H
#define CAPTUREMANAGER_H

#include "qmainwindow.h"
#include <QObject>
#include <QImage>
#include <QProcess>

class QSharedMemory;

// --- DEFINICIONES GLOBALES (Deben coincidir con Python) ---

const long SHM_SYSV_KEY = 123456;

struct ShmHeader {
    qlonglong status;   // 0 = Idle, 1 = Capturing
    qlonglong width;
    qlonglong height;
    qlonglong screen_index; // Para un nombre de hasta 39 chars + terminador NUL
};

const int HEADER_OFFSET = 32; // Los datos del frame empiezan DESPUES de estos 64 bytes.


// --- Declaracion de la Clase ---

class CaptureManager : public QObject
{
    Q_OBJECT

private:
    explicit CaptureManager(QObject *parent = 0);

public:
    static CaptureManager& instance();

    // --- Borramos constructores de copia para asegurar instancia unica ---
    CaptureManager(const CaptureManager&) = delete;
    void operator=(const CaptureManager&) = delete;
    ~CaptureManager();

//Variables
private:
    static bool m_isWayland;
    static QMainWindow *mainWindow;

    bool m_isCleanedUp;

    QProcess* m_process;
    QSharedMemory* m_shm;
    qint64          m_pid;
    ShmHeader       m_lastHeader; // Contiene SIEMPRE el ultimo header leido

//Metodos
private:
    void startCaptureService();
    void cleanupCaptureService();

public:
    static bool isWaylandSession();
    static void init(QMainWindow *mainWindow);
    int getActiveScreenIndex(bool heroDrafting) const;
    void triggerDraftStart();
    void triggerDraftEnd();
    QImage getLatestFrame();

private slots:
    void onProcessStarted();
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    bool attachToShm();
    void onProcessError(QProcess::ProcessError error);
    void printProcessOutput();
    void printProcessError();
};

#endif // CAPTUREMANAGER_H
