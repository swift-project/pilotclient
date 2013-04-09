/*  Copyright (C) 2013 VATSIM Community / authors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef AVTRACK_H
#define AVTRACK_H
#include "blackmisc/pqangle.h"

using BlackMisc::PhysicalQuantities::CAngle;
using BlackMisc::PhysicalQuantities::CAngleUnit;

namespace BlackMisc
{

namespace Aviation
{

/*!
 * \brief Track as used in aviation, can be true or magnetic Track
 * \remarks Intentionally allowing +/- CAngle , and >= / <= CAngle.
 */
class CTrack : public CAngle
{
private:
    bool m_magnetic; //!< magnetic or true Track?

protected:
    /*!
     * \brief Specific stream operation for Track
     * \return
     */
    virtual QString stringForStreamingOperator() const;

public:
    /*!
     * \brief Default constructor: 0 Track true
     */
    CTrack() : CAngle(0, CAngleUnit::rad()), m_magnetic(true) {}
    /*!
     * \brief Constructor
     * \param value
     * \param magnetic
     * \param unit
     */
    CTrack(double value, bool magnetic, const CAngleUnit &unit) : CAngle(value, unit), m_magnetic(magnetic) {}
    /*!
     * \brief Constructor
     * \param value
     * \param magnetic
     * \param unit
     */
    CTrack(int value, bool magnetic, const CAngleUnit &unit) : CAngle(value, unit), m_magnetic(magnetic) {}
    /*!
     * \brief Constructor by CAngle
     * \param Track
     * \param magnetic
     */
    CTrack(CAngle track, bool magnetic) : CAngle(), m_magnetic(magnetic) {
        CAngle::operator =(track);
    }
    /*!
     * \brief Copy constructor
     * \param otherTrack
     */
    CTrack(const CTrack &otherTrack) : CAngle(otherTrack), m_magnetic(otherTrack.m_magnetic) {}
    /*!
     * \brief Assignment operator =
     * \param otherQuantity
     * @return
     */
    CTrack &operator =(const CTrack &otherTrack);
    /*!
     * \brief Equal operator ==
     * \param otherQuantity
     * @return
     */
    bool operator ==(const CTrack &otherTrack);
    /*!
     * \brief Unequal operator ==
     * \param otherQuantity
     * @return
     */
    bool operator !=(const CTrack &otherTrack);
    /*!
     * \brief Magnetic Track?
     * \return
     */
    bool isMagneticTrack() const {
        return this->m_magnetic;
    }
    /*!
     * \brief True Track?
     * \return
     */
    bool isTrueTrack() const {
        return !this->m_magnetic;
    }
};

} // namespace

} // namespace

#endif // AVTRACK_H
