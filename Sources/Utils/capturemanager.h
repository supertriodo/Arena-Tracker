#ifndef CAPTUREMANAGER_H
#define CAPTUREMANAGER_H

#include <QObject>
#include <QImage>
#include <QProcess>

class QSharedMemory;

// --- DEFINICIONES GLOBALES (Deben coincidir con Python) ---

const long SHM_SYSV_KEY = 123456;

/**
 * @brief Estructura del encabezado (Header).
 * DEBE coincidir con el struct.pack("qqq40s") de Python.
 * qlonglong (8 bytes) * 3 = 24 bytes.
 * char[40] (40 bytes).
 * Total = 64 bytes.
 */
struct ShmHeader {
    qlonglong status;   // 0 = Idle, 1 = Capturing
    qlonglong width;
    qlonglong height;
    char screen_name[40]; // Para un nombre de hasta 39 chars + terminador NUL
};

const int HEADER_OFFSET = 64; // Los datos del frame empiezan DESPUES de estos 64 bytes.


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

    QProcess* m_process;
    QSharedMemory* m_shm;
    qint64          m_pid;
    ShmHeader       m_lastHeader; // Contiene SIEMPRE el ultimo header leido

//Metodos
private:
    void startCaptureService();
    void cleanupCaptureService();
    void triggerDraftStart();
    void triggerDraftEnd();
    bool isCapturing() const;
    QString getActiveScreenName() const;
    int getActiveScreenIndex() const;

public:
    static bool isWaylandSession();
    static void init();

    QImage getLatestFrame();

private slots:
    void onProcessStarted();
    bool attachToShm();
    void onProcessError(QProcess::ProcessError error);
    void printProcessOutput();
    void printProcessError();
};

#endif // CAPTUREMANAGER_H
