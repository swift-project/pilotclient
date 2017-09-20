/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_DB_DISTRIBUTIONLIST_H
#define BLACKMISC_DB_DISTRIBUTIONLIST_H

#include "distribution.h"
#include "blackmisc/db/datastoreobjectlist.h"
#include "blackmisc/datacache.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"
#include "blackmisc/variant.h"
#include <QSet>
#include <QString>

namespace BlackMisc
{
    namespace Db
    {
        //! Multiple distributions for different channels:
        //! - one CDistribution objects contains all versions for a channel
        //! - a distribution list normally contains all distributions for all channels
        class BLACKMISC_EXPORT CDistributionList :
            public BlackMisc::CSequence<CDistribution>,
            public BlackMisc::Db::IDatastoreObjectList<CDistribution, CDistributionList, int>,
            public BlackMisc::Mixin::MetaType<CDistributionList>
        {
        public:
            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CDistributionList)

            //! Empty constructor.
            CDistributionList();

            //! Construct from a base class object.
            CDistributionList(const CSequence<CDistribution> &other);

            //! All channels
            QSet<QString> getChannels() const;

            //! Find channels for platform
            QSet<QString> findChannelsForPlatform(const QString &platform) const;

            //! All platforms for all channels
            QSet<QString> getPlatforms() const;

            //! Find distribution by channels
            CDistribution findByChannelOrDefault(const QString &channel) const;

            //! Version for specific channel and platform
            QString getVersionForChannelAndPlatform(const QString &channel, const QString &platform) const;

            //! Version for specific channel and platform
            QVersionNumber getQVersionForChannelAndPlatform(const QString &channel, const QString &platform) const;

            //! Version for specific channel and platform
            QString getVersionForChannelAndPlatform(const QStringList &channelPlatform) const;

            //! Version for specific channel and platform
            QVersionNumber getQVersionForChannelAndPlatform(const QStringList &channelPlatform) const;

            //! Guess the best channel/platform
            QStringList guessMyDefaultChannelAndPlatform() const;

            //! From database JSON by array
            static CDistributionList fromDatabaseJson(const QJsonArray &array);

            //! From database JSON by string
            static CDistributionList fromDatabaseJson(const QString &json);
        };

        //! Trait for distributions
        struct TDistributionsInfo : public BlackMisc::TDataTrait<CDistributionList>
        {
            //! Key in data cache
            static const char *key() { return "distributions"; }

            //! First load is synchronous
            static constexpr bool isPinned() { return true; }
        };
    } // ns
} // ns

Q_DECLARE_METATYPE(BlackMisc::Db::CDistributionList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Db::CDistribution>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Db::CDistribution>)

#endif //guard
