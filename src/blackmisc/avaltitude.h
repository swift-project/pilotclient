/*  Copyright (C) 2013 VATSIM Community / authors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_AVALTITUDE_H
#define BLACKMISC_AVALTITUDE_H
#include "blackmisc/pqlength.h"

namespace BlackMisc
{
namespace Aviation
{

/*!
 * \brief Altitude as used in aviation, can be AGL or MSL altitude
 * \remarks Intentionally allowing +/- CLength , and >= / <= CLength.
 */
class CAltitude : public BlackMisc::PhysicalQuantities::CLength
{
private:
    bool m_msl; //!< MSL or AGL?

protected:
    /*!
     * \brief Specific stream operation for Altitude
     * \return
     */
    virtual QString stringForConverter() const;

public:
    /*!
     * \brief Default constructor: 0 Altitude true
     */
    CAltitude() : BlackMisc::PhysicalQuantities::CLength(0, BlackMisc::PhysicalQuantities::CLengthUnit::m()), m_msl(true) {}
    /*!
     * \brief Constructor
     * \param value
     * \param msl MSL or AGL?
     * \param unit
     */
    CAltitude(double value, bool msl, const BlackMisc::PhysicalQuantities::CLengthUnit &unit) : BlackMisc::PhysicalQuantities::CLength(value, unit), m_msl(msl) {}
    /*!
     * \brief Constructor
     * \param value
     * \param msl MSL or AGL?
     * \param unit
     */
    CAltitude(int value, bool msl, const BlackMisc::PhysicalQuantities::CLengthUnit &unit) : CLength(value, unit), m_msl(msl) {}
    /*!
     * \brief Constructor by CLength
     * \param altitude
     * \param msl
     */
    CAltitude(BlackMisc::PhysicalQuantities::CLength altitude, bool msl) : BlackMisc::PhysicalQuantities::CLength(), m_msl(msl)
    {
        BlackMisc::PhysicalQuantities::CLength::operator =(altitude);
    }
    /*!
     * \brief Copy constructor
     * \param otherAltitude
     */
    CAltitude(const CAltitude &otherAltitude) : BlackMisc::PhysicalQuantities::CLength(otherAltitude), m_msl(otherAltitude.m_msl) {}
    /*!
     * \brief Assignment operator =
     * \param otherAltitude
     * @return
     */
    CAltitude &operator =(const CAltitude &otherAltitude);
    /*!
     * \brief Equal operator ==
     * \param otherAltitude
     * @return
     */
    bool operator ==(const CAltitude &otherAltitude);
    /*!
     * \brief Unequal operator ==
     * \param otherAltitude
     * @return
     */
    bool operator !=(const CAltitude &otherAltitude);
    /*!
     * \brief AGL Above ground level?
     * \return
     */
    bool isAboveGroundLevel() const
    {
        return !this->m_msl;
    }
    /*!
     * \brief MSL Mean sea level?
     * \return
     */
    bool isMeanSeaLevel() const
    {
        return this->m_msl;
    }
};

} // namespace
} // namespace

#endif // BLACKMISC_AVALTITUDE_H
