// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/compressutils.h"

#include <QFileInfo>
#include <QProcess>

#include "config/buildconfig.h"
#include "misc/fileutils.h"
#include "misc/stringutils.h"
#include "misc/swiftdirectories.h"

using namespace swift::config;

namespace swift::misc
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

    bool CCompressUtils::zipUncompress(const QString &file, const QString &directory, QStringList *stdOutAndError)
    {
        const QFileInfo fi(file);
        if (!fi.exists()) { return false; }
        if (fi.suffix() != "zip")
        {
            if (stdOutAndError) { stdOutAndError->push_back("Not a zip file"); }
            return false;
        }

        const bool win = CBuildConfig::isRunningOnWindowsNtPlatform();
        const QString d = directory.isEmpty() ? directory : win ? CFileUtils::toWindowsLocalPath(directory) : directory;
        const QString f = win ? CFileUtils::toWindowsLocalPath(file) : file;

        QProcess zipProcess;

        if constexpr (CBuildConfig::isRunningOnWindowsNtPlatform())
        {
            zipProcess.setProgram("powershell");
            QStringList args;
            args << "-Command";
            args << "Expand-Archive";
            args << "-Path" << f;
            if (!d.isEmpty()) { args << "-DestinationPath" << d; }
            args << "-Force";
            zipProcess.setArguments(args);
        }
        else
        {
            zipProcess.setProgram("unzip");
            QStringList args;
            args << f;
            if (!d.isEmpty()) { args << "-d" << d; }
            zipProcess.setArguments(args);
        }
        return runZipProcess(&zipProcess, stdOutAndError);
    }

    bool CCompressUtils::runZipProcess(QProcess *zipProcess, QStringList *stdOutAndError)
    {
        zipProcess->start();

        // If process does not even start, e.g. because unzip program found.
        if (!zipProcess->waitForStarted())
        {
            if (stdOutAndError)
            {
                stdOutAndError->push_back("unzip");
                stdOutAndError->push_back("Command not found");
            }
            return false;
        }

        // If process does not finish. Very unlikely.
        if (!zipProcess->waitForFinished())
        {
            if (stdOutAndError)
            {
                stdOutAndError->push_back("unzip");
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

        return zipProcess->exitStatus() == QProcess::NormalExit && zipProcess->exitCode() == 0;
    }
} // namespace swift::misc
