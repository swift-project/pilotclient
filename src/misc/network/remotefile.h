// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_NETWORK_REMOTEFILE_H
#define SWIFT_MISC_NETWORK_REMOTEFILE_H

#include <QJsonObject>
#include <QMetaType>
#include <QString>

#include "misc/metaclass.h"
#include "misc/network/url.h"
#include "misc/propertyindexref.h"
#include "misc/swiftmiscexport.h"
#include "misc/timestampbased.h"
#include "misc/valueobject.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::network, CRemoteFile)

namespace swift::misc::network
{
    /*!
     * Remote file, i.e. a file residing on a server
     */
    class SWIFT_MISC_EXPORT CRemoteFile :
        public CValueObject<CRemoteFile>,
        public ITimestampBased
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexName = CPropertyIndexRef::GlobalIndexCRemoteFile,
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

        //! \copydoc swift::misc::CFileUtils::isExecutableFile
        bool isExecutableFile() const;

        //! \copydoc swift::misc::CFileUtils::isSwiftInstaller
        bool isSwiftInstaller() const;

        //! Set URL
        void setUrl(const CUrl &url) { m_url = url; }

        //! Set URL
        void setUrl(const QString &url);

        //! Get size
        qint64 getSize() const { return m_size; }

        //! \copydoc swift::misc::CFileUtils::humanReadableFileSize
        QString getSizeHumanReadable() const;

        //! Set size
        void setSize(qint64 size) { m_size = size; }

        //! Created timestamp
        QString getFormattedCreatedYmdhms() const;

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(swift::misc::CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(swift::misc::CPropertyIndexRef index, const QVariant &variant);

        //! Role from DB JSON
        static CRemoteFile fromDatabaseJson(const QJsonObject &json);

    private:
        QString m_name;
        QString m_description;
        CUrl m_url;
        qint64 m_size = 0;
        qint64 m_created = 0;

        SWIFT_METACLASS(
            CRemoteFile,
            SWIFT_METAMEMBER(timestampMSecsSinceEpoch),
            SWIFT_METAMEMBER(name),
            SWIFT_METAMEMBER(description),
            SWIFT_METAMEMBER(url),
            SWIFT_METAMEMBER(size),
            SWIFT_METAMEMBER(created));
    };
} // namespace swift::misc::network

Q_DECLARE_METATYPE(swift::misc::network::CRemoteFile)

#endif // guard
