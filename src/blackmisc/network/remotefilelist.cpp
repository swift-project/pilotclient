/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/network/remotefilelist.h"
#include "blackmisc/network/remotefile.h"
#include "blackmisc/json.h"

#include <QJsonValue>

BLACK_DEFINE_SEQUENCE_MIXINS(BlackMisc::Network, CRemoteFile, CRemoteFileList)

namespace BlackMisc::Network
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
        return CRemoteFileList::fromDatabaseJson(Json::jsonArrayFromString(json));
    }
} // namespace
