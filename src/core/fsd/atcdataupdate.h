// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_FSD_ATCPOSITION_H
#define SWIFT_CORE_FSD_ATCPOSITION_H

#include <QtGlobal>

#include "core/fsd/enums.h"
#include "core/fsd/messagebase.h"
#include "misc/network/facilitytype.h"

namespace swift::core::fsd
{
    //! FSD Message: ATC data update
    class SWIFT_CORE_EXPORT AtcDataUpdate : public MessageBase
    {
        friend bool operator==(const AtcDataUpdate &, const AtcDataUpdate &);

    public:
        //! Constructor
        AtcDataUpdate(const QString &sender, int frequencykHz, swift::misc::network::CFacilityType facility, int visibleRange, AtcRating rating,
                      double latitude, double longitude, int elevation);

        //! Message converted to tokens
        QStringList toTokens() const;

        //! Construct from tokens
        static AtcDataUpdate fromTokens(const QStringList &tokens);

        //! @{
        //! Properties
        int m_frequencykHz = 0.0;
        swift::misc::network::CFacilityType m_facility;
        int m_visibleRange = 0.0;
        AtcRating m_rating = AtcRating::Unknown;
        double m_latitude = 0.0;
        double m_longitude = 0.0;
        int m_elevation = 0.0;
        //! @}

        //! PDU identifier
        static QString pdu() { return "%"; }

    private:
        //! Ctor
        AtcDataUpdate();
    };

    //! Equal to operator
    inline bool operator==(const AtcDataUpdate &lhs, const AtcDataUpdate &rhs)
    {
        return lhs.sender() == rhs.sender() &&
               lhs.receiver() == rhs.receiver() &&
               lhs.m_frequencykHz == rhs.m_frequencykHz &&
               lhs.m_facility == rhs.m_facility &&
               lhs.m_visibleRange == rhs.m_visibleRange &&
               lhs.m_rating == rhs.m_rating &&
               qFuzzyCompare(1 + lhs.m_latitude, 1 + rhs.m_latitude) &&
               qFuzzyCompare(1 + lhs.m_longitude, 1 + rhs.m_longitude) &&
               lhs.m_elevation == rhs.m_elevation;
    }

    //! Not equal to operator
    inline bool operator!=(const AtcDataUpdate &lhs, const AtcDataUpdate &rhs)
    {
        return !(lhs == rhs);
    }
} // namespace swift::core::fsd

#endif // guard
