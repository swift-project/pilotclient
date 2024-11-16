//  SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
//  SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/aviation/ongroundinfo.h"

#include "misc/verify.h"

SWIFT_DEFINE_VALUEOBJECT_MIXINS(swift::misc::aviation, COnGroundInfo)

using namespace swift::misc::physical_quantities;

namespace swift::misc::aviation
{

    COnGroundInfo::COnGroundInfo(IsOnGround onGround, OnGroundDetails details)
        : m_onGroundDetails(static_cast<int>(details))
    {
        switch (onGround)
        {
        case IsOnGround::OnGroundSituationUnknown: m_onGroundFactor = -1.0; break;
        case IsOnGround::OnGround: m_onGroundFactor = 1.0; break;
        case IsOnGround::NotOnGround: m_onGroundFactor = 0.0; break;
        }
    }

    const QString &COnGroundInfo::isOnGroundToString(IsOnGround onGround)
    {
        static const QString notog("not on ground");
        static const QString og("on ground");
        static const QString unknown("unknown");

        switch (onGround)
        {
        case IsOnGround::NotOnGround: return notog;
        case IsOnGround::OnGround: return og;
        case IsOnGround::OnGroundSituationUnknown:
        default: return unknown;
        }
    }

    const QString &COnGroundInfo::onGroundDetailsToString(OnGroundDetails reliability)
    {
        static const QString elvCg("elevation/CG");
        static const QString interpolation("interpolation");
        static const QString guess("guessing");
        static const QString unknown("unknown");
        static const QString outOwnAircraft("own aircraft");
        static const QString inNetwork("from network");
        static const QString inFromParts("from parts");

        switch (reliability)
        {
        case OnGroundDetails::OnGroundByElevationAndCG: return elvCg;
        case OnGroundDetails::OnGroundByGuessing: return guess;
        case OnGroundDetails::OnGroundByInterpolation: return interpolation;
        case OnGroundDetails::OutOnGroundOwnAircraft: return outOwnAircraft;
        case OnGroundDetails::InFromNetwork: return inNetwork;
        case OnGroundDetails::InFromParts: return inFromParts;
        case OnGroundDetails::NotSetGroundDetails:
        default: return unknown;
        }
    }

    void COnGroundInfo::registerMetadata()
    {
        CValueObject<COnGroundInfo>::registerMetadata();
        qRegisterMetaType<IsOnGround>();
        qRegisterMetaType<OnGroundDetails>();
    }

    COnGroundInfo::COnGroundInfo(double interpolatedGndFactor)
        : m_onGroundDetails(static_cast<int>(OnGroundDetails::OnGroundByInterpolation)),
          m_onGroundFactor(interpolatedGndFactor)
    {
        // Clip small ground factor values
        if (m_onGroundFactor < 0.0) { m_onGroundFactor = -1.0; }
        else if (m_onGroundFactor < 0.001) { m_onGroundFactor = 0.0; }
        else if (m_onGroundFactor > 0.999) { m_onGroundFactor = 1.0; }
    }

    COnGroundInfo::COnGroundInfo(const CLength &cg, const CLength &groundDistance)
    {
        m_onGroundDetails = static_cast<int>(OnGroundDetails::OnGroundByElevationAndCG);
        if (groundDistance.isNull()) { m_onGroundFactor = -1.0; }
        else if (groundDistance.isNegativeWithEpsilonConsidered()) { m_onGroundFactor = 1.0; }
        else if (groundDistance.abs() < deltaNearGround()) { m_onGroundFactor = 1.0; }
        else if (!cg.isNull())
        {
            // smaller than percentage from CG
            const CLength cgFactor(cg * 0.1);
            if (groundDistance.abs() < cgFactor.abs()) { m_onGroundFactor = 1.0; }
        }
        m_onGroundFactor = 0.0;
    }

    bool COnGroundInfo::isOnGround() const
    {
        SWIFT_VERIFY_X(m_onGroundFactor >= 0.0, Q_FUNC_INFO, "Should only be called with positive groundfactors");
        if (m_onGroundDetails == OnGroundDetails::OnGroundByInterpolation)
        {
            return m_onGroundFactor > m_groundFactorThreshold;
        }
        else { return math::CMathUtils::epsilonEqual(m_onGroundFactor, 1.0); }
    }

    COnGroundInfo::IsOnGround COnGroundInfo::getOnGround() const
    {
        if (this->m_onGroundFactor < 0.0) { return OnGroundSituationUnknown; }

        const bool onGround = isOnGround();
        return onGround ? OnGround : NotOnGround;
    }

    COnGroundInfo::OnGroundDetails COnGroundInfo::getGroundDetails() const
    {
        return static_cast<COnGroundInfo::OnGroundDetails>(m_onGroundDetails);
    }

    const CLength &COnGroundInfo::deltaNearGround()
    {
        static const CLength small(0.5, CLengthUnit::m());
        return small;
    }

    QString COnGroundInfo::convertToQString(bool /*i18n*/) const
    {
        return u" | factor: " % QString::number(m_onGroundFactor, 'f', 2) % u" | source: " %
               onGroundDetailsToString(static_cast<OnGroundDetails>(m_onGroundDetails));
    }

    QVariant COnGroundInfo::propertyByIndex(CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }

        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexOnGroundFactor: return QVariant::fromValue(m_onGroundFactor);
        case IndexOnGroundDetails: return QVariant::fromValue(m_onGroundDetails);
        default: return CValueObject::propertyByIndex(index);
        }
    }

    void COnGroundInfo::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself())
        {
            (*this) = variant.value<COnGroundInfo>();
            return;
        }

        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexOnGroundFactor: m_onGroundFactor = variant.toDouble(); break;
        case IndexOnGroundDetails: m_onGroundDetails = variant.toInt(); break;
        default: CValueObject::setPropertyByIndex(index, variant); break;
        }
    }
} // namespace swift::misc::aviation
