// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_DB_DISTRIBUTION_H
#define SWIFT_MISC_DB_DISTRIBUTION_H

#include <QMetaType>
#include <QString>
#include <QVersionNumber>

#include "config/buildconfig.h"
#include "misc/db/datastore.h"
#include "misc/dictionary.h"
#include "misc/network/url.h"
#include "misc/settingscache.h"
#include "misc/swiftmiscexport.h"
#include "misc/valueobject.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::db, CDistribution)

namespace swift::misc::db
{
    //! Distributions for channel
    class SWIFT_MISC_EXPORT CDistribution : public CValueObject<CDistribution>, public IDatastoreObjectWithIntegerKey
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexChannel = CPropertyIndexRef::GlobalIndexCDistribution,
            IndexStability,
            IndexRestricted,
            IndexDownloadUrl
        };

        //! Default constructor
        CDistribution() = default;

        //! Constructor
        CDistribution(const QString &channel, int stability, bool restricted);

        //! Version channel (Alpha, Beta, Stable ..)
        const QString &getChannel() const { return m_channel; }

        //! Set the channel
        void setChannel(const QString &channel);

        //! Stability (higher is more stable)
        int getStability() const { return m_stability; }

        //! Order
        void setStability(int stability) { m_stability = stability; }

        //! Download URL, i.e. here one can download installer
        const network::CUrl &getDownloadUrl() const { return m_downloadUrl; }

        //! Set URL, ignored if empty
        void setDownloadUrl(const swift::misc::network::CUrl &url);

        //! Has a download URL?
        bool hasDownloadUrl() const;

        //! Restricted channel?
        bool isRestricted() const { return m_restricted; }

        //! Restricted channel
        void setRestricted(bool r) { m_restricted = r; }

        //! Get the restrict icon
        CIcons::IconIndex getRestrictionIcon() const;

        //! "this" having same or better stability than other distribution?
        bool isStabilitySameOrBetter(const CDistribution &otherDistribution) const;

        //! "this" having better stability than other distribution?
        bool isStabilityBetter(const CDistribution &otherDistribution) const;

        //! Empty?
        bool isEmpty() const { return m_channel.isEmpty(); }

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! To string
        QString convertToQString(const QString &separator, bool i18n = false) const;

        //! Representing icon
        CIcons::IconIndex toIcon() const;

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

        //! Pseudo distribution for local builds
        const static CDistribution &localDeveloperBuild();

        //! Object from database JSON format
        static CDistribution fromDatabaseJson(const QJsonObject &json, const QString &prefix = {});

    private:
        QString m_channel; //!< channel the files belong to
        int m_stability; //!< stability
        bool m_restricted = false; //!< restricted access (i.e. password for download needed)
        network::CUrl m_downloadUrl; //!< download URL, here I get the installer

        SWIFT_METACLASS(
            CDistribution,
            SWIFT_METAMEMBER(dbKey),
            SWIFT_METAMEMBER(timestampMSecsSinceEpoch),
            SWIFT_METAMEMBER(channel),
            SWIFT_METAMEMBER(stability),
            SWIFT_METAMEMBER(downloadUrl));
    };
} // namespace swift::misc::db

Q_DECLARE_METATYPE(swift::misc::db::CDistribution)

#endif // SWIFT_MISC_DB_DISTRIBUTION_H
