#include "capturemanager.h"
#include "qscreen.h"

// --- Includes de Implementacion (los que no necesita el .h) ---
#include <QSharedMemory> // Header completo, necesario para usar m_shm->attach(), lock(), etc.
#include <QDebug>        // Para qDebug()
#include <QByteArray>      // Para qgetenv()
#include <QTimer>        // Para el reintento de attachToShm
#include <QGuiApplication> // Necesario para QGuiApplication::screens()

// Cabeceras POSIX para kill()
extern "C" {
#include <signal.h>
#include <sys/types.h>
}

bool CaptureManager::m_isWayland;

// --- IMPLEMENTACION DE LAS FUNCIONES ---

CaptureManager& CaptureManager::instance()
{
    // Creada una vez y solo una (thread-safe en C++11 y posteriores)
    static CaptureManager staticInstance;
    return staticInstance;
}

CaptureManager::CaptureManager(QObject *parent)
    : QObject(parent),
    m_process(Q_NULLPTR),
    m_shm(Q_NULLPTR),
    m_pid(0)
{
    m_lastHeader.status = 0;

    // Le pasamos a QSharedMemory la clave numerica convertida a String.
    // Asi es como QSharedMemory sabe que debe usar la clave numerica de System V
    // (123456) en lugar de crear un segmento POSIX basado en el texto "123456".
    // Esto asegura que C++ y Python se conectan AL MISMO segmento.
    m_shm = new QSharedMemory(QString::number(SHM_SYSV_KEY));
}

CaptureManager::~CaptureManager()
{
    cleanupCaptureService();

    delete m_process;
    delete m_shm;
}

void CaptureManager::init()
{
    QByteArray sessionType = qgetenv("XDG_SESSION_TYPE");
    qDebug() << "XDG_SESSION_TYPE:" << sessionType;
    CaptureManager::m_isWayland = (sessionType == "wayland");
    if(CaptureManager::m_isWayland) CaptureManager::instance().startCaptureService();
}

bool CaptureManager::isWaylandSession()
{
    return CaptureManager::m_isWayland;
}

void CaptureManager::startCaptureService()
{
    if (m_process) {
        qWarning() << "El servicio de captura ya esta iniciado.";
        return;
    }

    if (!m_isWayland) {
        qWarning() << "Error: Esta solucion esta disenada para Wayland. Sesion X11 detectada.";
        // Aqui llamarias a tu logica/script de fallback para X11
        return;
    }

    m_process = new QProcess();

    connect(m_process, &QProcess::started, this, &CaptureManager::onProcessStarted);
    connect(m_process, SIGNAL(errorOccurred(QProcess::ProcessError)), this, SLOT(onProcessError(QProcess::ProcessError)));
    connect(m_process, &QProcess::readyReadStandardOutput, this, &CaptureManager::printProcessOutput);
    connect(m_process, &QProcess::readyReadStandardError, this, &CaptureManager::printProcessError);

    QString program = "python3";
    QStringList args;

    // --- IMPORTANTE: Cambia esto a la ruta absoluta de tu script ---
    args << "/ruta/absoluta/completa/a/capture_service_wayland.py";

    qDebug() << "Iniciando servicio de captura Wayland:" << program << args;
    m_process->start(program, args);
}

void CaptureManager::onProcessStarted()
{
    m_pid = m_process->processId();
    qDebug() << "Servicio de Python iniciado con exito. PID:" << m_pid;

    // El script Python crea la SHM al arrancar. Intentamos adjuntarnos.
    if (!attachToShm()) {
        qWarning() << "Primer intento de attach fallo. Reintentando en 500ms...";
        // Es comun que el proceso 'started' se dispare antes de que el script haya creado la SHM.
        // Usamos un singleShot para reintentar.
        QTimer::singleShot(500, this, SLOT(attachToShm()));
    }
}

bool CaptureManager::attachToShm()
{
    if (m_shm->isAttached()) {
        return true;
    }

    if (!m_shm->attach(QSharedMemory::ReadOnly)) {
        qDebug() << "Aun no se pudo adjuntar a SHM:" << m_shm->errorString();
        return false;
    }

    qDebug() << "¡Adjuntado a segmento SHM (Key" << SHM_SYSV_KEY << ") exitosamente!";
    return true;
}

void CaptureManager::triggerDraftStart()
{
    if (m_pid <= 0) {
        qWarning() << "No se puede iniciar draft: PID invalido (proceso no iniciado?).";
        return;
    }
    qDebug() << "Enviando señal SIGUSR1 (START) al PID" << m_pid;
    int result = kill(static_cast<pid_t>(m_pid), SIGUSR1);
    if (result != 0) {
        qWarning() << "Fallo al enviar señal SIGUSR1! (errno:" << errno << ")";
    }
}

void CaptureManager::triggerDraftEnd()
{
    if (m_pid <= 0) {
        qWarning() << "No se puede detener draft: PID invalido.";
        return;
    }
    qDebug() << "Enviando señal SIGUSR2 (STOP) al PID" << m_pid;
    int result = kill(static_cast<pid_t>(m_pid), SIGUSR2);
    if (result != 0) {
        qWarning() << "Fallo al enviar señal SIGUSR2! (errno:" << errno << ")";
    }
}

void CaptureManager::cleanupCaptureService()
{
    qDebug() << "Limpiando servicio de captura...";
    if (m_shm && m_shm->isAttached()) {
        m_shm->detach();
    }

    if (m_process && m_process->state() == QProcess::Running) {
        m_process->terminate(); // Envia SIGTERM (que el script captura limpiamente)
        if (!m_process->waitForFinished(2000)) {
            qWarning() << "El script no termino en 2s, forzando kill.";
            m_process->kill(); // Envia SIGKILL
        }
        qDebug() << "Proceso de script detenido.";
    }
    // El destructor de QObject (parent) se encargara de borrar m_process y m_shm
    m_process = Q_NULLPTR; // Prevenir doble borrado
}

QImage CaptureManager::getLatestFrame()
{
    if (!m_shm || !m_shm->isAttached()) {
        return QImage(); // No estamos listos
    }

    // Bloquear SHM para una lectura segura (atomica)
    m_shm->lock();

    // 1. Leer el encabezado
    ShmHeader currentHeader;
    memcpy(&currentHeader, m_shm->constData(), sizeof(ShmHeader));

    // 2. Comprobar estado
    if (currentHeader.status != 1 || currentHeader.width <= 0 || currentHeader.height <= 0) {
        m_shm->unlock();
        if (m_lastHeader.status == 1) { // Si *acaba* de parar
            qDebug() << "Stream detenido (leido status 0 desde header).";
        }
        m_lastHeader = currentHeader;
        return QImage();
    }

    m_lastHeader = currentHeader; // Guardamos el estado actual

    // 3. Obtener el puntero a los DATOS del frame (con offset)
    const uchar* frameDataPtr = static_cast<const uchar*>(m_shm->constData()) + HEADER_OFFSET;

    // 4. Crear la QImage (como wrapper, sin copia aun)
    // wf-recorder envia BGRA. En Qt (little-endian) QImage::Format_ARGB32 lo interpreta correctamente.
    QImage frame(frameDataPtr,
                 static_cast<int>(currentHeader.width),
                 static_cast<int>(currentHeader.height),
                 QImage::Format_ARGB32);

    // 5. IMPORTANTE: DEVOLVER UNA COPIA.
    // Tan pronto como hagamos unlock(), el script puede sobrescribir la memoria.
    QImage copy = frame.copy();

    m_shm->unlock();

    return copy;
}

bool CaptureManager::isCapturing() const
{
    // Devuelve el ultimo estado conocido (evita bloquear la SHM solo para esta comprobacion)
    return (m_lastHeader.status == 1);
}


// --- Slots de Debug ---
void CaptureManager::onProcessError(QProcess::ProcessError error)
{
    qWarning() << "Error en QProcess:" << error << m_process->errorString();
}

void CaptureManager::printProcessOutput()
{
    QByteArray data = m_process->readAllStandardOutput();
    if (!data.isEmpty()) {
        qDebug() << "[Python STDOUT]:" << data.trimmed(); // trimmed() quita saltos de linea basura
    }
}

void CaptureManager::printProcessError()
{
    QByteArray data = m_process->readAllStandardError();
    if (!data.isEmpty()) {
        qWarning() << "[Python STDERR]:" << data.trimmed();
    }
}

QString CaptureManager::getActiveScreenName() const
{
    if (isCapturing()) {
        // m_lastHeader.screen_name es un 'char[40]' leido desde la SHM.
        // Esta garantizado que es terminado en NUL por el script de Python (struct.pack lo hace).
        // Creamos un QString a partir de el.
        return QString::fromUtf8(m_lastHeader.screen_name);
    }
    return QString(); // Devolver string nulo si no se esta capturando
}

int CaptureManager::getActiveScreenIndex() const
{
    const QString activeName = getActiveScreenName();
    if (activeName.isEmpty()) {
        return -1; // No hay captura activa
    }

    // Obtener la lista de pantallas ACTUAL de la aplicacion Qt
    const QList<QScreen*> screens = QGuiApplication::screens();

    // Iterar para encontrar la coincidencia por NOMBRE
    for (int i = 0; i < screens.size(); ++i) {

        // Comparamos el nombre de la pantalla de Qt (ej. "eDP-1")
        // con el nombre que nos dio wf-recorder via Python (ej. "eDP-1").
        if (screens.at(i) != Q_NULLPTR && screens.at(i)->name() == activeName) {
            return i; // ¡Encontrado! Devolvemos el indice de la lista de Qt.
        }
    }

    qWarning() << "CaptureManager: No se pudo encontrar el indice para la pantalla activa!"
               << "Nombre buscado:" << activeName;

    // Imprimir nombres disponibles (para debug)
    for (int i = 0; i < screens.size(); ++i) {
        if (screens.at(i)) {
            qDebug() << "  Pantalla disponible [" << i << "]:" << screens.at(i)->name();
        }
    }

    return -1; // No se encontro
}
