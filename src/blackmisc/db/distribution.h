/* Copyright (C) 2017
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_DB_DISTRIBUTION_H
#define BLACKMISC_DB_DISTRIBUTION_H

#include "blackconfig/buildconfig.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/network/urllist.h"
#include "blackmisc/db/datastore.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/dictionary.h"
#include "blackmisc/valueobject.h"

#include <QMetaType>
#include <QVersionNumber>
#include <QString>

namespace BlackMisc
{
    namespace Db
    {
        //! Distributions for channel
        class BLACKMISC_EXPORT CDistribution :
            public CValueObject<CDistribution>,
            public IDatastoreObjectWithIntegerKey
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexChannel = CPropertyIndexRef::GlobalIndexCDistribution,
                IndexStability,
                IndexRestricted,
                IndexDownloadUrls
            };

            //! Default constructor
            CDistribution();

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

            //! Download URLs, i.e. here one can download installer
            const Network::CUrlList &getDownloadUrls() const { return m_downloadUrls; }

            //! Add URL, ignored if empty
            void addDownloadUrl(const Network::CUrl &url);

            //! At least one download URL?
            bool hasDownloadUrls() const;

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

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

            //! To string
            QString convertToQString(const QString &separator, bool i18n = false) const;

            //! Representing icon
            CIcons::IconIndex toIcon() const;

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            QVariant propertyByIndex(CPropertyIndexRef index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

            //! Pseudo distribution for local builds
            const static CDistribution &localDeveloperBuild();

            //! Object from database JSON format
            static CDistribution fromDatabaseJson(const QJsonObject &json, const QString &prefix = {});

        private:
            QString m_channel;                //!< channel the files belong to
            int     m_stability;              //!< stability
            bool    m_restricted = false;     //!< restricted access (i.e. password for download needed)
            Network::CUrlList m_downloadUrls; //!< download URLs, here I get the installer

            BLACK_METACLASS(
                CDistribution,
                BLACK_METAMEMBER(dbKey),
                BLACK_METAMEMBER(timestampMSecsSinceEpoch),
                BLACK_METAMEMBER(channel),
                BLACK_METAMEMBER(stability),
                BLACK_METAMEMBER(downloadUrls)
            );
        };
    } // ns
} // ns

Q_DECLARE_METATYPE(BlackMisc::Db::CDistribution)

#endif // guard
