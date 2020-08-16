/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_NETWORK_REMOTEFILE_H
#define BLACKMISC_NETWORK_REMOTEFILE_H

#include "url.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/timestampbased.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/variant.h"

#include <QJsonObject>
#include <QMetaType>
#include <QString>

namespace BlackMisc
{
    namespace Network
    {
        /*!
         * Remote file, i.e. a file residing on a server
         */
        class BLACKMISC_EXPORT CRemoteFile :
            public CValueObject<CRemoteFile>,
            public ITimestampBased
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexName = CPropertyIndex::GlobalIndexCRemoteFile,
                IndexDescription,
                IndexUrl,
                IndexSize
            };

            //! Constructor
            CRemoteFile() = default;

            //! Constructor
            CRemoteFile(const QString &name, const QString &description);

            //! Constructor
            CRemoteFile(const QString &name, qint64 size);

            //! Constructor
            CRemoteFile(const QString &name, qint64 size, const QString &url);

            //! Name
            const QString &getName() const { return m_name; }

            //! Name with directory stripped
            QString getBaseName() const { return m_name.section('/', -1); }

            //! Has name?
            bool hasName() const { return !m_name.isEmpty(); }

            //! Name + human readable size
            QString getBaseNameAndSize() const;

            //! Name
            void setName(const QString &name) { m_name = name.trimmed(); }

            //! Matching name?
            bool matchesBaseName(const QString &baseName) const;

            //! Description
            const QString &getDescription() const { return m_description; }

            //! Description
            void setDescription(const QString &description) { m_description = description.trimmed(); }

            //! Get URL
            const CUrl &getUrl() const { return m_url; }

            //! Has an URL
            bool hasUrl() const { return !m_url.isEmpty(); }

            //! Automatically concatenates the name if missing
            CUrl getSmartUrl() const;

            //! File with appendix
            bool isFileWithSuffix() const { return this->getUrl().isFileWithSuffix(); }

            //! \copydoc BlackMisc::CFileUtils::isExecutableFile
            bool isExecutableFile() const;

            //! \copydoc BlackMisc::CFileUtils::isSwiftInstaller
            bool isSwiftInstaller() const;

            //! Set URL
            void setUrl(const CUrl &url) { m_url = url; }

            //! Set URL
            void setUrl(const QString &url);

            //! Get size
            qint64 getSize() const { return m_size; }

            //! \copydoc BlackMisc::CFileUtils::humanReadableFileSize
            QString getSizeHumanReadable() const;

            //! Set size
            void setSize(qint64 size) { m_size = size; }

            //! Created timestamp
            QString getFormattedCreatedYmdhms() const;

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(const BlackMisc::CPropertyIndex &index, const CVariant &variant);

            //! Role from DB JSON
            static CRemoteFile fromDatabaseJson(const QJsonObject &json);

        private:
            QString m_name;
            QString m_description;
            CUrl m_url;
            qint64 m_size = 0;
            qint64 m_created = 0;

            BLACK_METACLASS(
                CRemoteFile,
                BLACK_METAMEMBER(timestampMSecsSinceEpoch),
                BLACK_METAMEMBER(name),
                BLACK_METAMEMBER(description),
                BLACK_METAMEMBER(url),
                BLACK_METAMEMBER(size),
                BLACK_METAMEMBER(created)
            );
        };
    } // ns
} // ns

Q_DECLARE_METATYPE(BlackMisc::Network::CRemoteFile)

#endif // guard
