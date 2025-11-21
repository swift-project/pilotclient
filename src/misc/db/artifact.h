// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_DB_ARTIFACT_H
#define SWIFT_MISC_DB_ARTIFACT_H

#include <QString>

#include "misc/db/datastore.h"
#include "misc/db/distributionlist.h"
#include "misc/network/remotefile.h"
#include "misc/platform.h"
#include "misc/swiftmiscexport.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::db, CArtifact)

namespace swift::misc::db
{
    //! Artifacts ("our software" products)
    class SWIFT_MISC_EXPORT CArtifact : public CValueObject<CArtifact>, public IDatastoreObjectWithIntegerKey
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexName = CPropertyIndexRef::GlobalIndexCArtifact,
            IndexMd5,
            IndexType,
            IndexPlatform,
            IndexVersionString,
            IndexQVersion,
            IndexSize,
            IndexSizeHumanReadable,
            IndexDistributions
        };

        //! Type
        enum ArtifactType
        {
            UnknownArtifact,
            PilotClientInstaller,
            Symbols,
            XSwiftBus
        };

        //! Default constructor
        CArtifact() = default;

        //! Constructor
        CArtifact(const QString &name, const QString &version, const QString &md5, ArtifactType type, int size,
                  bool existing, const CPlatform &platform);

        //! Having name?
        bool hasName() const { return !m_name.isEmpty(); }

        //! Name (i.e. installer name, symbol name)
        const QString &getName() const { return m_name; }

        //! Set the name
        void setName(const QString &name) { m_name = name.trimmed(); }

        //! Matching name?
        bool matchesName(const QString &name, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;

        //! Get type
        ArtifactType getType() const { return static_cast<ArtifactType>(m_type); }

        //! Unknown
        bool isUnknown() const { return this->getType() == UnknownArtifact; }

        //! Set type
        void setType(ArtifactType type) { m_type = static_cast<int>(type); }

        //! File size
        int getFileSize() const { return m_size; }

        //! \copydoc CFileUtils::humanReadableFileSize
        QString getFileSizeHumanReadable() const;

        //! Set file size
        void setFileSize(int size) { m_size = size; }

        //! MD5
        const QString &getMd5() const { return m_md5; }

        //! OS
        const CPlatform &getPlatform() const { return m_platform; }

        //! Matches any platform
        bool matchesAnyPlatform(const CPlatform &platform) const;

        //! Set the OS
        void setPlatform(const CPlatform &platform) { m_platform = platform; }

        //! Related distributions
        const CDistributionList &getDistributions() const { return m_distributions; }

        //! Most stable distribution if any
        CDistribution getMostStableDistribution() const { return this->getDistributions().getMostStableOrDefault(); }

        //! Related distributions
        void setDistributions(const CDistributionList &distributions) { m_distributions = distributions; }

        //! Has distributions?
        bool hasDistributions() const { return !m_distributions.isEmpty(); }

        //! Has unrestricted distribution
        bool hasUnrestrictedDistribution() const;

        //! Is distributed with given distribution?
        bool isWithDistribution(const CDistribution &distribution, bool acceptMoreStableDistributions) const;

        //! Turn into remote file
        //! \note requires distributions
        network::CRemoteFile asRemoteFile() const;

        //! Newer than the current build
        bool isNewerThanCurrentBuild() const;

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! To string
        QString convertToQString(const QString &separator, bool i18n = false) const;

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(swift::misc::CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(swift::misc::CPropertyIndexRef index, const QVariant &variant);

        //! Object from database JSON format
        static CArtifact fromDatabaseJson(const QJsonObject &json, const QString &prefix = {});

        //! Type as string
        static const QString &typeToString(ArtifactType type);

        //! Name to platform
        static CPlatform artifactNameToPlatform(const QString &name);

    private:
        QString m_name; //!< filename
        QString m_md5; //!< MD5 checksum
        int m_type = static_cast<int>(UnknownArtifact); //!< artifact type
        int m_size = -1; //!< size in bytes
        bool m_existing = false; //!< existing artifact for download
        CPlatform m_platform; //!< platform (i.e. OS)
        CDistributionList m_distributions; //!< related distributions

        //! Extract version number from a file name
        static QString versionNumberFromFilename(const QString &filename);

        //! Type from string
        static ArtifactType stringToType(const QString &str);

        //! Trim the 4th segment of a version string
        static QString trimVersionString(const QString &version);

        //! Trim a string representing 4th segment
        static QString trim4thSegment(const QString &seg);

        SWIFT_METACLASS(
            CArtifact,
            SWIFT_METAMEMBER(name),
            SWIFT_METAMEMBER(version),
            SWIFT_METAMEMBER(md5),
            SWIFT_METAMEMBER(type),
            SWIFT_METAMEMBER(size),
            SWIFT_METAMEMBER(existing),
            SWIFT_METAMEMBER(platform),
            SWIFT_METAMEMBER(distributions),
            SWIFT_METAMEMBER(timestampMSecsSinceEpoch));
    };
} // namespace swift::misc::db

Q_DECLARE_METATYPE(swift::misc::db::CArtifact)
Q_DECLARE_METATYPE(swift::misc::db::CArtifact::ArtifactType)

#endif // SWIFT_MISC_DB_ARTIFACT_H
