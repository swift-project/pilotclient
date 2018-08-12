/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "compressutils.h"
#include "blackconfig/buildconfig.h"
#include "fileutils.h"
#include "directoryutils.h"
#include "stringutils.h"

#include <QFileInfo>
#include <QProcess>

using namespace BlackConfig;

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

    bool CCompressUtils::zip7Uncompress(const QString &file, const QString &directory, bool wait, QStringList *stdOutAndError)
    {
        const QFileInfo fi(file);
        if (!fi.exists()) { return false; }
        if (!CCompressUtils::hasZip7(stdOutAndError)) { return false; }

        const bool win = CBuildConfig::isRunningOnWindowsNtPlatform();
        const QString d =
            directory.isEmpty() ? directory :
            win ? CFileUtils::toWindowsLocalPath(directory) : directory;
        const QString f = win ? CFileUtils::toWindowsLocalPath(file) : file;

        // 7za.exe x -o"P:\Temp\XPlane" c:\Users\Foo\Downloads\xswiftbus-allos-0.8.4.802111947.7z
        const QString cmd = win ? QStringLiteral("7za.exe") : QStringLiteral("7za");
        QStringList args;
        args << "x";
        args << "-aoa";
        if (!d.isEmpty()) { args << "-o" + d; }
        args << f;

        if (wait)
        {
            QProcess zipProcess;
            zipProcess.start(cmd, args);
            const bool finished = zipProcess.waitForFinished();
            if (zipProcess.exitStatus() != QProcess::NormalExit) { return false; }
            if (!finished) { return false; }
            const int r = zipProcess.exitCode();
            if (stdOutAndError)
            {
                const QString pStdout = zipProcess.readAllStandardOutput();
                const QString pStderr = zipProcess.readAllStandardError();
                stdOutAndError->clear();
                stdOutAndError->push_back(pStdout);
                stdOutAndError->push_back(pStderr);
            }
            return r == 0;
        }
        else
        {
            QProcess *p = new QProcess();
            p->start(cmd, args);
            return true;
        }
    }

    bool CCompressUtils::hasZip7(QStringList *stdOutAndError)
    {
        // just display info
        const bool win = CBuildConfig::isRunningOnWindowsNtPlatform();
        if (!win) { return CCompressUtils::whichZip7(stdOutAndError); }

        // windows check
        QStringList args;
        args << "i";
        QProcess zipProcess;
        zipProcess.start("7za.exe", args);
        const bool finished = zipProcess.waitForFinished();
        if (stdOutAndError)
        {
            stdOutAndError->clear();
            const QString pStdout = zipProcess.readAllStandardOutput();
            const QString pStderr = zipProcess.readAllStandardError();
            if (pStdout.isEmpty() && pStderr.isEmpty())
            {
                stdOutAndError->push_back("Checking 7za");
                stdOutAndError->push_back("No 7za or failing");
            }
            else
            {
                stdOutAndError->push_back(pStdout);
                stdOutAndError->push_back(pStderr);
            }
        }

        if (zipProcess.exitStatus() != QProcess::NormalExit) { return false; }
        if (!finished) { return false; }
        const int r = zipProcess.exitCode();
        return r == 0;
    }

    bool CCompressUtils::whichZip7(QStringList *stdOutAndError)
    {
        const QString cmd("which 7za");
        QProcess zipProcess;
        zipProcess.start(cmd);
        const bool finished = zipProcess.waitForFinished();
        if (!finished) { return false; }

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
} // ns
