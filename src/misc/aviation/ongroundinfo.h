//  SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
//  SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_AVIATION_ONGROUNDINFO_H
#define SWIFT_MISC_AVIATION_ONGROUNDINFO_H

#include "misc/swiftmiscexport.h"
#include "misc/valueobject.h"
#include "misc/pq/length.h"

BLACK_DECLARE_VALUEOBJECT_MIXINS(swift::misc::Aviation, COnGroundInfo)

namespace swift::misc::aviation
{
    //! Information about the ground status
    class SWIFT_MISC_EXPORT COnGroundInfo : public CValueObject<COnGroundInfo>
    {
    public:
        //! Is on ground?
        enum IsOnGround
        {
            NotOnGround,
            OnGround,
            OnGroundSituationUnknown
        };

        //! Reliability of on ground information
        enum OnGroundDetails
        {
            NotSetGroundDetails,
            // interpolated situation
            OnGroundByInterpolation, //!< strongest for remote aircraft
            OnGroundByElevationAndCG,
            OnGroundByGuessing, //!< weakest
            // received situation
            InFromNetwork, //!< received from network
            InFromParts, //!< set from aircraft parts
            // send information
            OutOnGroundOwnAircraft //!< sending on ground
        };

        //! Properties by index
        enum ColumnIndex
        {
            IndexOnGroundFactor = CPropertyIndexRef::GlobalIndexCOnGroundInfo,
            IndexOnGroundDetails,
        };

        COnGroundInfo() = default;

        //! Create GroundInfo with fixed decision (on ground, not on ground or not known) and with
        //! info about the source of this knowledge
        COnGroundInfo(IsOnGround onGround, OnGroundDetails details);

        //! Create GroundInfo from information about CG and distance from ground
        COnGroundInfo(const physical_quantities::CLength &cg, const physical_quantities::CLength &groundDistance);

        //! Create GroundInfo from interpolated ground factor
        explicit COnGroundInfo(double interpolatedGndFactor);

        //! Get the ground factor
        //! Use this for interpolation only!!
        //! For just checking if the info is OnGround or NotOnGround use the getOnGround() method instead.
        double getGroundFactor() const { return m_onGroundFactor; }

        //! When source of knowledge changes
        void setOnGroundDetails(OnGroundDetails details)
        {
            // TODO Assert not by interpolation
            m_onGroundDetails = static_cast<int>(details);
        }

        //! Is on ground?
        //! \return IsOnGround state of this object
        IsOnGround getOnGround() const;

        //! Get ground details
        //! \return ground details of this object
        OnGroundDetails getGroundDetails() const;

        //! \copydoc mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! \copydoc mixin::Index::propertyByIndex
        QVariant propertyByIndex(CPropertyIndexRef index) const;

        //! \copydoc mixin::Index::setPropertyByIndex
        void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

        //! Register metadata
        static void registerMetadata();

        //! Enum to string
        static const QString &isOnGroundToString(IsOnGround onGround);

        //! Enum to string
        static const QString &onGroundDetailsToString(OnGroundDetails reliability);

        //! Delta distance, near to ground
        static const physical_quantities::CLength &deltaNearGround();

    private:
        //! Check if the aircraft is considered to be on the ground.
        //! Depending on the data source, different definitions are used on when the aircraft
        //! is considered to be on the ground.
        //! This method should only be called when the m_onGroundFactor is >= 0.0, as it is only does a binary
        //! decision.
        //! \return true, if the aircraft is considered to be on the ground
        bool isOnGround() const;

        int m_onGroundDetails = static_cast<int>(OnGroundDetails::NotSetGroundDetails);
        double m_onGroundFactor = -1.0; //!< interpolated ground flag, 1..on ground, 0..not on ground, -1 no info

        static constexpr double m_groundFactorThreshold = 0.95; //!< With m_onGroundDetails == "OnGroundByInterpolation", this is the threshold used to decide if the ground factor is OnGround or NotOnGround

        BLACK_METACLASS(
        COnGroundInfo,
        BLACK_METAMEMBER(onGroundDetails),
        BLACK_METAMEMBER(onGroundFactor));
    };

}

Q_DECLARE_METATYPE(swift::misc::aviation::COnGroundInfo)
Q_DECLARE_METATYPE(swift::misc::aviation::COnGroundInfo::IsOnGround)
Q_DECLARE_METATYPE(swift::misc::aviation::COnGroundInfo::OnGroundDetails)

#endif // SWIFT_MISC_AVIATION_ONGROUNDINFO_H
