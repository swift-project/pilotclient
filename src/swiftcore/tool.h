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

        //! Server loop
        static void serverLoop(BlackCore::CRuntime *runtime);

    };

} // namespace

#endif // guard
