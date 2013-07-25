/*  Copyright (C) 2013 VATSIM Community / authors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_AVLATERALPOSITION_H
#define BLACKMISC_AVLATERALPOSITION_H

#include "blackmisc/avaltitude.h"
#include "blackmisc/pqconstants.h"

namespace BlackMisc
{
namespace Aviation
{

/*!
 * \brief Vertical (Z) positions of an aircraft
 */
class CAviationVerticalPositions
{
    /*!
     * \brief Stream << overload to be used in debugging messages
     * \param d
     * \param positions
     * \return
     */
    friend QDebug operator<<(QDebug d, const CAviationVerticalPositions &positions);

    /*!
     * Stream operator for log messages
     * \brief operator <<
     * \param log
     * \param positions
     * \return
     */
    friend CLogMessage operator<<(CLogMessage log, const CAviationVerticalPositions &positions);

private:
    BlackMisc::Aviation::CAltitude m_altitude;  //!< altitude
    BlackMisc::PhysicalQuantities::CLength m_elevation;  //!< elevation
    BlackMisc::PhysicalQuantities::CLength m_height;  //!< height

protected:
    /*!
     * \brief Specific stream operation for heading
     * \return
     */
    virtual QString stringForStreamingOperator() const;

public:
    /*!
     * \brief Default constructor
     */
    CAviationVerticalPositions();
    /*!
     * \brief Constructor
     * \param altitude
     * \param elevation
     * \param height
     */
    CAviationVerticalPositions(const CAltitude &altitude, const BlackMisc::PhysicalQuantities::CLength &elevation, const BlackMisc::PhysicalQuantities::CLength &height);
    /*!
     * \brief Copy constructor
     * \param otherPosition
     */
    CAviationVerticalPositions(const CAviationVerticalPositions &otherPosition);
    /*!
     * \brief Assignment operator =
     * \param otherPositions
     * @return
     */
    CAviationVerticalPositions &operator =(const CAviationVerticalPositions &otherPositions);
    /*!
     * \brief Equal operator ==
     * \param otherPositions
     * @return
     */
    bool operator ==(const CAviationVerticalPositions &otherPositions);
    /*!
     * \brief Unequal operator ==
     * \param otherPositions
     * @return
     */
    bool operator !=(const CAviationVerticalPositions &otherPositions);
    /*!
     * \brief Height
     * \return
     */
    BlackMisc::PhysicalQuantities::CLength getHeight() const
    {
        return this->m_height;
    }
    /*!
     * \brief Elevation
     * \return
     */
    BlackMisc::PhysicalQuantities::CLength getElevation() const
    {
        return this->m_elevation;
    }
    /*!
     * \brief Altitude
     * \return
     */
    CAltitude getAltitude()const
    {
        return this->m_altitude;
    }
    /*!
     * \brief Factory getting tupel frome levation and altitude values in ft
     * \param altitudeMslFt
     * \param elevationFt
     * \return
     */
    static CAviationVerticalPositions fromAltitudeAndElevationInFt(double altitudeMslFt, double elevationFt);
    /*!
     * \brief Factory getting tupel frome levation and altitude values in meters
     * \param altitudeMslM
     * \param elevationM
     * \return
     */
    static CAviationVerticalPositions fromAltitudeAndElevationInM(double altitudeMslM, double elevationM);
};

} // namespace
} // namespace

#endif // BLACKMISC_AVLATERALPOSITION_H
