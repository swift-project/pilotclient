#include "testservicetool.h"

namespace BlackMiscTest
{

/*
 * Start a new process
 */
QProcess * TestserviceTool::startNewProcess(const QString &executable, QObject *parent = 0)
{
    QProcess *process = new QProcess(parent);
    process->startDetached(executable);
    return process;
}

/*
 * Sleep
 */
void TestserviceTool::sleep(qint32 ms)
{
    if (ms < 1) return;

#ifdef Q_OS_WIN
    Sleep(uint(ms));
#else
    struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
    nanosleep(&ts, NULL);
#endif

}

} // namespace
