/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_TIMESTAMPOBJECTLIST_H
#define BLACKMISC_TIMESTAMPOBJECTLIST_H

#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"
#include <QList>
#include <QMap>

namespace BlackMisc
{
    //! List of objects with timestamp.
    //! Such objects should implement \sa ITimestampBased
    template<class OBJ, class CONTAINER>
    class ITimestampObjectList
    {
    public:

        //! List of objects before dateTime
        CONTAINER findBefore(const QDateTime &dateTime) const;

        //! List of objects before msSinceEpoch
        CONTAINER findBefore(qint64 msSinceEpoch) const;

        //! List of objects after dateTime
        CONTAINER findAfter(const QDateTime &dateTime) const;

        //! List of objects after msSinceEpoch
        CONTAINER findAfter(qint64 msSinceEpoch) const;

        //! Split into 2 containers, [0] >= msSinceEpoch [b] < msSinceEpoch
        QList<CONTAINER> splitByTime(qint64 msSinceEpoch) const;

        //! Latest value
        OBJ latestValue() const;

        //! Latest value
        OBJ oldestValue() const;

        //! Remove objects with timestamp before dateTime
        void removeBefore(const QDateTime &dateTime);

        //! Remove objects with timestamp before dateTime
        void removeBefore(qint64 msSinceEpoch);

        //! Remove objects older than seconds
        void removeOlderThanNowMinusOffset(int offsetMs);

        //! Sort by timestamp
        void sortLatestFirst();

        //! Sort by timestamp
        void sortOldestFirst();

    protected:
        //! Constructor
        ITimestampObjectList();

        //! Container
        virtual const CONTAINER &getContainer() const = 0;

        //! Container
        virtual CONTAINER &getContainer() = 0;
    };

} //namespace

#endif //guard
