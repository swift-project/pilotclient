#ifndef BLACKMISCTEST_Tool_H
#define BLACKMISCTEST_Tool_H

#include "blackcore/coreruntime.h"
#include <QCoreApplication>
#include <QProcess>
#include <QDBusConnection>

namespace BlackMiscTest
{

    /*!
     * \brief Supporting functions for running the tests
     */
    class Tool
    {
    private:
        /*!
         * \brief Constructor
         */
        Tool() {}

    public:
        /*!
         * \brief Get process id
         * \return
         */
        static qint64 getPid()
        {
            return QCoreApplication::applicationPid();
        }

        /*!
         * \brief Start a new process
         * \param executable
         * \param arguments
         * \param parent
         * \return
         */
        static QProcess *startNewProcess(const QString &executable, const QStringList &arguments = QStringList(), QObject *parent = 0);

        /*!
         * \brief Server loop
         * \param core
         */
        static void serverLoop(const BlackCore::CCoreRuntime *core);
    };

} // namespace

#endif // guard
