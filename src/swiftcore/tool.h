#ifndef BLACKMISCTEST_Tool_H
#define BLACKMISCTEST_Tool_H

#include "blackcore/context_runtime.h"
#include <QCoreApplication>
#include <QProcess>
#include <QDBusConnection>

namespace BlackMiscTest
{

    /*!
     * Supporting functions for running the tests
     */
    class Tool
    {
    private:
        //! Constructor
        Tool() {}

    public:
        //! Get process id
        static qint64 getPid()
        {
            return QCoreApplication::applicationPid();
        }

        //! Start a new process
        static QProcess *startNewProcess(const QString &executable, const QStringList &arguments = QStringList(), QObject *parent = 0);

        //! Server loop
        static void serverLoop(BlackCore::CRuntime *runtime);

    };

} // namespace

#endif // guard
