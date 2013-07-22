#ifndef BLACKMISCTEST_TESTSERVICETOOL_H
#define BLACKMISCTEST_TESTSERVICETOOL_H

#include <QCoreApplication>
#include <QProcess>
#ifdef Q_OS_WIN
// for qdatetime, see here http://qt-project.org/forums/viewthread/22133
#define NOMINMAX
#include <windows.h> // for Sleep
#endif

namespace BlackMiscTest
{

/*!
 * \brief Supporting functions for running the tests
 */
class TestserviceTool
{
private:
    /*!
     * \brief Constructor
     */
    TestserviceTool() {}

public:
    /*!
     * \brief Get process id
     * \return
     */
    static qint64 getPid() {
        return QCoreApplication::applicationPid();
    }

    /*!
     * \brief Start a new process
     * \param executable
     * \param parent
     * \return
     */
    static QProcess *startNewProcess(const QString &executable, QObject *parent);

    /*!
     * \brief Own sleep to avoid inlude of QTest
     * \param ms
     */
    static void sleep(qint32 ms);

};

} // namespace

#endif // BLACKMISCTEST_TESTSERVICETOOL_H
