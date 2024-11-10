// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/network/remotefilelist.h"
#include "misc/network/remotefile.h"
#include "misc/json.h"

#include <QJsonValue>

SWIFT_DEFINE_SEQUENCE_MIXINS(swift::misc::network, CRemoteFile, CRemoteFileList)

namespace swift::misc::network
{
    CRemoteFileList::CRemoteFileList() {}

    CRemoteFileList::CRemoteFileList(const CSequence<CRemoteFile> &other) : CSequence<CRemoteFile>(other)
    {}

    QStringList CRemoteFileList::getNames(bool sorted) const
    {
        QStringList fileNames;
        for (const CRemoteFile &rf : *this)
        {
            fileNames.append(rf.getName());
        }
        if (sorted) { fileNames.sort(); }
        return fileNames;
    }

    QStringList CRemoteFileList::getBaseNamesPlusSize(bool sorted) const
    {
        QStringList fileNames;
        for (const CRemoteFile &rf : *this)
        {
            fileNames.append(rf.getBaseNameAndSize());
        }
        if (sorted) { fileNames.sort(); }
        return fileNames;
    }

    CRemoteFile CRemoteFileList::findFirstByNameOrDefault(const QString &name) const
    {
        if (name.isEmpty()) { return CRemoteFile(); }
        return this->findFirstByOrDefault(&CRemoteFile::getName, name);
    }

    CRemoteFile CRemoteFileList::findFirstContainingNameOrDefault(const QString &name, Qt::CaseSensitivity cs) const
    {
        if (name.isEmpty()) { return CRemoteFile(); }
        for (const CRemoteFile &rf : *this)
        {
            if (rf.getName().contains(name, cs)) { return rf; }
        }
        return CRemoteFile();
    }

    CRemoteFile CRemoteFileList::findFirstByMatchingBaseNameOrDefault(const QString &baseName) const
    {
        if (baseName.isEmpty()) { return CRemoteFile(); }
        for (const CRemoteFile &rf : *this)
        {
            if (rf.matchesBaseName(baseName)) { return rf; }
        }
        return CRemoteFile();
    }

    CRemoteFileList CRemoteFileList::findExecutableFiles() const
    {
        CRemoteFileList files;
        for (const CRemoteFile &rf : *this)
        {
            if (CFileUtils::isExecutableFile(rf.getName()))
            {
                files.push_back(rf);
            }
        }
        return files;
    }

    qint64 CRemoteFileList::getTotalFileSize() const
    {
        qint64 s = 0;
        for (const CRemoteFile &rf : *this)
        {
            s += rf.getSize();
        }
        return s;
    }

    QString CRemoteFileList::getTotalFileSizeHumanReadable() const
    {
        return CFileUtils::humanReadableFileSize(this->getTotalFileSize());
    }

    CRemoteFileList CRemoteFileList::fromDatabaseJson(const QJsonArray &array)
    {
        CRemoteFileList roles;
        for (const QJsonValue &value : array)
        {
            roles.push_back(CRemoteFile::fromDatabaseJson(value.toObject()));
        }
        return roles;
    }

    CRemoteFileList CRemoteFileList::fromDatabaseJson(const QString &json)
    {
        if (json.isEmpty()) { return CRemoteFileList(); }
        return CRemoteFileList::fromDatabaseJson(json::jsonArrayFromString(json));
    }
} // namespace
