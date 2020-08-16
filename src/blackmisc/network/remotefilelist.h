/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_NETWORK_REMOTEFILELIST_H
#define BLACKMISC_NETWORK_REMOTEFILELIST_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/network/remotefile.h"
#include "blackmisc/sequence.h"
#include "blackmisc/variant.h"

#include <QJsonArray>
#include <QMetaType>
#include <QString>
#include <QStringList>

namespace BlackMisc
{
    namespace Network
    {
        //! Value object encapsulating a list of remote files.
        class BLACKMISC_EXPORT CRemoteFileList :
            public CSequence<CRemoteFile>,
            public Mixin::MetaType<CRemoteFileList>
        {
        public:
            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CRemoteFileList)
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
    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Network::CRemoteFileList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Network::CRemoteFile>)

#endif //guard
