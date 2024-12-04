// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_DB_DISTRIBUTIONLIST_H
#define SWIFT_MISC_DB_DISTRIBUTIONLIST_H

#include <QSet>
#include <QString>

#include "misc/collection.h"
#include "misc/db/datastoreobjectlist.h"
#include "misc/db/distribution.h"
#include "misc/platform.h"
#include "misc/sequence.h"
#include "misc/swiftmiscexport.h"

SWIFT_DECLARE_SEQUENCE_MIXINS(swift::misc::db, CDistribution, CDistributionList)

namespace swift::misc::db
{
    //! Multiple distributions for different channels:
    //! - one CDistribution objects contains all artifacts for a channel
    //! - a distribution list normally contains all artifacts for all channels
    //! \sa CArtifact
    class SWIFT_MISC_EXPORT CDistributionList :
        public CSequence<CDistribution>,
        public IDatastoreObjectList<CDistribution, CDistributionList, int>,
        public mixin::MetaType<CDistributionList>
    {
    public:
        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CDistributionList)
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
} // namespace swift::misc::db

Q_DECLARE_METATYPE(swift::misc::db::CDistributionList)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::misc::db::CDistribution>)

#endif // SWIFT_MISC_DB_DISTRIBUTIONLIST_H
