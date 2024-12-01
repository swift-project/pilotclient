// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_NETWORK_REMOTEFILELIST_H
#define SWIFT_MISC_NETWORK_REMOTEFILELIST_H

#include <QJsonArray>
#include <QMetaType>
#include <QString>
#include <QStringList>

#include "misc/collection.h"
#include "misc/network/remotefile.h"
#include "misc/sequence.h"
#include "misc/swiftmiscexport.h"

SWIFT_DECLARE_SEQUENCE_MIXINS(swift::misc::network, CRemoteFile, CRemoteFileList)

namespace swift::misc::network
{
    //! Value object encapsulating a list of remote files.
    class SWIFT_MISC_EXPORT CRemoteFileList : public CSequence<CRemoteFile>, public mixin::MetaType<CRemoteFileList>
    {
    public:
        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CRemoteFileList)
        using CSequence::CSequence;

        //! Default constructor.
        CRemoteFileList();

        //! Construct from a base class object.
        CRemoteFileList(const CSequence<CRemoteFile> &other);

        //! All file names
        QStringList getNames(bool sorted = true) const;

        //! All file names plus size
        QStringList getBaseNamesPlusSize(bool sorted = true) const;

        //! First by name of default
        CRemoteFile findFirstByNameOrDefault(const QString &name) const;

        //! First by name contained of default
        CRemoteFile findFirstContainingNameOrDefault(const QString &name, Qt::CaseSensitivity cs) const;

        //! Find first matching name of default
        CRemoteFile findFirstByMatchingBaseNameOrDefault(const QString &name) const;

        //! Find all executable files (decided by appendix)
        CRemoteFileList findExecutableFiles() const;

        //! Size of all files
        qint64 getTotalFileSize() const;

        //! Size formatted
        QString getTotalFileSizeHumanReadable() const;

        //! From our database JSON format
        static CRemoteFileList fromDatabaseJson(const QJsonArray &array);

        //! From our database JSON format
        static CRemoteFileList fromDatabaseJson(const QString &json);
    };
} // namespace swift::misc::network

Q_DECLARE_METATYPE(swift::misc::network::CRemoteFileList)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::misc::network::CRemoteFile>)

#endif // SWIFT_MISC_NETWORK_REMOTEFILELIST_H
