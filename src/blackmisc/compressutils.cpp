// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/compressutils.h"
#include "blackmisc/fileutils.h"
#include "blackmisc/swiftdirectories.h"
#include "blackmisc/stringutils.h"
#include "config/buildconfig.h"

#include <QFileInfo>
#include <QProcess>

using namespace swift::config;

namespace BlackMisc
{
    QByteArray CCompressUtils::lengthHeader(qint32 size)
    {
        // Length header, unsigned, big-endian, 32-bit integer
        QByteArray lengthHeader;
        QDataStream stream(&lengthHeader, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::BigEndian);
        stream << size;
        Q_ASSERT_X(lengthHeader.size() == 4, Q_FUNC_INFO, "Wrong header size");
        return lengthHeader;
    }

    //! Returns the platform specific 7za command
    QString getZip7Executable()
    {
        QString executable;
        if (CBuildConfig::isRunningOnMacOSPlatform())
        {
            executable += CSwiftDirectories::binDirectory();
            executable += '/';
        }
        executable += QStringLiteral("7za");
        return executable;
    }

    bool CCompressUtils::zip7Uncompress(const QString &file, const QString &directory, QStringList *stdOutAndError)
    {
        const QFileInfo fi(file);
        if (!fi.exists()) { return false; }
        if (!CCompressUtils::hasZip7(stdOutAndError)) { return false; }

        const bool win = CBuildConfig::isRunningOnWindowsNtPlatform();
        const QString d =
            directory.isEmpty() ? directory :
            win                 ? CFileUtils::toWindowsLocalPath(directory) :
                                  directory;
        const QString f = win ? CFileUtils::toWindowsLocalPath(file) : file;

        // 7za.exe x -o"P:\Temp\XPlane" c:\Users\Foo\Downloads\xswiftbus-allos-0.8.4.802111947.7z

        QStringList args;
        args << "x";
        args << "-aoa";
        if (!d.isEmpty()) { args << "-o" + d; }
        args << f;

        QProcess zipProcess;
        zipProcess.setProgram(getZip7Executable());
        zipProcess.setArguments(args);
        return runZip7Process(&zipProcess, stdOutAndError);
    }

    bool CCompressUtils::hasZip7(QStringList *stdOutAndError)
    {
        // just display info
        if (CBuildConfig::isRunningOnLinuxPlatform())
        {
            return CCompressUtils::whichZip7(stdOutAndError);
        }

        QStringList args;
        args << "i";
        QProcess zipProcess;
        zipProcess.setProgram(getZip7Executable());
        zipProcess.setArguments(args);
        return runZip7Process(&zipProcess, stdOutAndError);
    }

    bool CCompressUtils::whichZip7(QStringList *stdOutAndError)
    {
        const QString cmd("which 7za");
        QProcess zipProcess;
        zipProcess.start(cmd);
        if (!zipProcess.waitForStarted()) { return false; }
        if (!zipProcess.waitForFinished()) { return false; }

        const QString pStdout = zipProcess.readAllStandardOutput();
        const QString pStderr = zipProcess.readAllStandardError();
        if (stdOutAndError)
        {
            stdOutAndError->clear();
            stdOutAndError->push_back(pStdout);
            stdOutAndError->push_back(pStderr);
        }
        const int r = zipProcess.exitCode();
        return r == 0 && pStdout.contains("7za", Qt::CaseInsensitive);
    }

    bool CCompressUtils::runZip7Process(QProcess *zipProcess, QStringList *stdOutAndError)
    {
        zipProcess->start();

        // If process does not even start, e.g. because no 7za exe found.
        if (!zipProcess->waitForStarted())
        {
            if (stdOutAndError)
            {
                stdOutAndError->push_back("7za");
                stdOutAndError->push_back("Command not found");
            }
            return false;
        }

        // If process does not finish. Very unlikely.
        if (!zipProcess->waitForFinished())
        {
            if (stdOutAndError)
            {
                stdOutAndError->push_back("7za");
                stdOutAndError->push_back("Process did not finish.");
            }
            return false;
        }

        if (stdOutAndError)
        {
            stdOutAndError->clear();
            const QString pStdout = zipProcess->readAllStandardOutput();
            const QString pStderr = zipProcess->readAllStandardError();
            stdOutAndError->push_back(pStdout);
            stdOutAndError->push_back(pStderr);
        }

        return zipProcess->exitStatus() == QProcess::NormalExit;
    }
} // ns
