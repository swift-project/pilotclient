// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_DB_ARTIFACT_H
#define BLACKMISC_DB_ARTIFACT_H

#include "blackmisc/db/distributionlist.h"
#include "blackmisc/db/datastore.h"
#include "blackmisc/platform.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/network/remotefile.h"
#include <QString>

BLACK_DECLARE_VALUEOBJECT_MIXINS(BlackMisc::Db, CArtifact)

namespace BlackMisc::Db
{
    //! Artifacts ("our software" products)
    class BLACKMISC_EXPORT CArtifact :
        public CValueObject<CArtifact>,
        public IDatastoreObjectWithIntegerKey
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
        CArtifact();

        //! Constructor
        CArtifact(const QString &name, const QString &version, const QString &md5,
                  ArtifactType type, int size, bool existing, const CPlatform &platform);

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
        Network::CRemoteFile asRemoteFile() const;

        //! Newer tahn the current build
        bool isNewerThanCurrentBuild() const;

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! To string
        QString convertToQString(const QString &separator, bool i18n = false) const;

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        QVariant propertyByIndex(BlackMisc::CPropertyIndexRef index) const;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(BlackMisc::CPropertyIndexRef index, const QVariant &variant);

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

        //! Trim a strin representing 4th segment
        static QString trim4thSegment(const QString &seg);

        BLACK_METACLASS(
            CArtifact,
            BLACK_METAMEMBER(name),
            BLACK_METAMEMBER(version),
            BLACK_METAMEMBER(md5),
            BLACK_METAMEMBER(type),
            BLACK_METAMEMBER(size),
            BLACK_METAMEMBER(existing),
            BLACK_METAMEMBER(platform),
            BLACK_METAMEMBER(distributions),
            BLACK_METAMEMBER(timestampMSecsSinceEpoch)
        );
    };
} // ns

Q_DECLARE_METATYPE(BlackMisc::Db::CArtifact)
Q_DECLARE_METATYPE(BlackMisc::Db::CArtifact::ArtifactType)

#endif // guard
