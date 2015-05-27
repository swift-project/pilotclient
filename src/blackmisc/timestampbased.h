/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_TIMESTAMPBASED_H
#define BLACKMISC_TIMESTAMPBASED_H

#include "blackmiscexport.h"
#include "propertyindex.h"
#include <QDateTime>

namespace BlackMisc
{

    //! Entity with timestamp
    class BLACKMISC_EXPORT ITimestampBased
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexUtcTimestamp = BlackMisc::CPropertyIndex::GlobalIndexTimestampBased,
            IndexUtcTimestampFormatted,
            IndexUtcTimestampFormattedHms,
            IndexUtcTimestampFormattedHm,
            IndexMSecsSinceEpoch // keep this as last item
        };

        //! Get timestamp
        QDateTime getUtcTimestamp() const;

        //! Timestamp as ms value
        qint64 getMSecsSinceEpoch() const { return m_timestampMSecsSinceEpoch; }

        //! Timestamp as ms value
        void setMSecsSinceEpoch(qint64 mSecsSinceEpoch) { m_timestampMSecsSinceEpoch = mSecsSinceEpoch; }

        //! Set timestamp
        void setUtcTimestamp(const QDateTime &timestamp);

        //! Is this newer than other?
        bool isNewerThan(const ITimestampBased &otherTimestampObj) const;

        //! Is newer than epoch value?
        bool isNewerThan(qint64 mSecsSinceEpoch) const;

        //! Is this older than other?
        bool isOlderThan(const ITimestampBased &otherTimestampObj) const;

        //! Is this older than other?
        bool isOlderThan(qint64 mSecsSinceEpoch) const;

        //! Older than now-offset
        bool isOlderThanNowMinusOffset(int offsetMs) const;

        //! Same timestamp
        bool isSame(const ITimestampBased &otherTimestampObj) const;

        //! Time difference
        qint64 msecsTo(const ITimestampBased &otherTimestampObj) const;

        //! Time difference
        qint64 absMsecsTo(const ITimestampBased &otherTimestampObj) const;

        //! Set the current time as timestamp
        void setCurrentUtcTime();

        //! Formatted timestamp
        QString getFormattedUtcTimestamp() const;

        //! As hh:mm:ss
        QString getFormattedUtcTimestampHms() const;

        //! As hh:mm
        QString getFormattedUtcTimestampHm() const;

        //! As YYYY mm dd hh ss
        QString getFormattedUtcTimestampYmdhms() const;

        //! As YYYY mm dd hh ss.zzz
        QString getFormattedUtcTimestampYmdhmsz() const;

        //! Can given index be handled
        static bool canHandleIndex(const BlackMisc::CPropertyIndex &index);

    protected:
        //! Destructor
        virtual ~ITimestampBased() {}

        //! \copydoc CValueObject::propertyByIndex
        virtual CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

        //! \copydoc CValueObject::setPropertyByIndex
        virtual void setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index);

        qint64 m_timestampMSecsSinceEpoch = QDateTime::currentMSecsSinceEpoch(); //!< timestamp value
    };

} // namespace

#endif // guard
