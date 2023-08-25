// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_DB_DISTRIBUTIONLIST_H
#define BLACKMISC_DB_DISTRIBUTIONLIST_H

#include "blackmisc/db/distribution.h"
#include "blackmisc/db/datastoreobjectlist.h"
#include "blackmisc/platform.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"
#include <QSet>
#include <QString>

BLACK_DECLARE_SEQUENCE_MIXINS(BlackMisc::Db, CDistribution, CDistributionList)

namespace BlackMisc::Db
{
    //! Multiple distributions for different channels:
    //! - one CDistribution objects contains all artifacts for a channel
    //! - a distribution list normally contains all artifacts for all channels
    //! \sa CArtifact
    class BLACKMISC_EXPORT CDistributionList :
        public CSequence<CDistribution>,
        public IDatastoreObjectList<CDistribution, CDistributionList, int>,
        public Mixin::MetaType<CDistributionList>
    {
    public:
        BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CDistributionList)
        using CSequence::CSequence;

        //! Empty constructor.
        CDistributionList();

        //! Construct from a base class object.
        CDistributionList(const CSequence<CDistribution> &other);

        //! All channels
        QStringList getChannels() const;

        //! Stability
        void sortByStability(Qt::SortOrder order = Qt::AscendingOrder);

        //! Contains distributions considered of same stability or more stable
        bool containsEqualOrMoreStable(CDistribution &distribution) const;

        //! Contains any unrestricted
        bool containsUnrestricted() const;

        //! Contains channel?
        bool containsChannel(const QString &channel) const;

        //! Find by channel
        CDistribution findFirstByChannelOrDefault(const QString &channel) const;

        //! Find by restriction flag
        CDistributionList findByRestriction(bool restricted) const;

        //! Most stable or default
        CDistribution getMostStableOrDefault() const;

        //! Least stable or default
        CDistribution getLeastStableOrDefault() const;

        //! From database JSON by array
        static CDistributionList fromDatabaseJson(const QJsonArray &array);

        //! From database JSON by string
        static CDistributionList fromDatabaseJson(const QString &json);
    };
} // ns

Q_DECLARE_METATYPE(BlackMisc::Db::CDistributionList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Db::CDistribution>)

#endif // guard
