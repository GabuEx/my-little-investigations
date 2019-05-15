#include <QApplication>
#include <QtPlugin>
#include <QtGlobal>
#include <QTime>

#include "UIComponents/MainWindow.h"
#include "Utilities/AudioPlayer.h"

#ifdef QT_DEBUG
void HandleQtMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg);
void HandleSignal(int signal);
#endif

int main(int argc, char *argv[])
{
#ifdef QT_DEBUG
    qInstallMessageHandler(HandleQtMessage);
    signal(SIGFPE, HandleSignal);
    signal(SIGILL, HandleSignal);
    signal(SIGSEGV, HandleSignal);
#endif

    QApplication application(argc, argv);

    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());

    MainWindow mainWindow;
    application.installEventFilter(&mainWindow);
    mainWindow.show();

    AudioPlayer::Initialize();
    int returnValue = application.exec();
    AudioPlayer::Cleanup();

    return returnValue;
}

#ifdef QT_DEBUG
void HandleQtMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();

    switch (type)
    {
    case QtDebugMsg:
        fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtWarningMsg:
        fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        abort();
    }
}

void HandleSignal(int signal)
{
    fprintf(stderr, "Interrupt signal: %d", signal);
    exit(signal);
}
#endif
