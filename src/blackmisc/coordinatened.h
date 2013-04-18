/*  Copyright (C) 2013 VATSIM Community / authors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_COORDINATENED_H
#define BLACKMISC_COORDINATENED_H
#include "blackmisc/mathvector3dbase.h"
#include "blackmisc/mathmatrix3x3.h"
#include "blackmisc/coordinategeodetic.h"

namespace BlackMisc
{
namespace Geo
{
/*!
 * \brief North, East, Down
 */
class CCoordinateNed : public BlackMisc::Math::CVector3DBase<CCoordinateNed>
{

private:
    CCoordinateGeodetic m_referencePosition; //!< geodetic reference position
    bool m_hasReferencePosition; //!< valid reference position?

public:
    /*!
     * \brief Default constructor
     */
    CCoordinateNed() : CVector3DBase(), m_hasReferencePosition(false) {}

    /*!
     * \brief Constructor with reference position
     * \param referencePosition
     */
    CCoordinateNed(const CCoordinateGeodetic &referencePosition) : CVector3DBase(), m_referencePosition(referencePosition), m_hasReferencePosition(true) {}

    /*!
     * \brief Constructor by values
     * \param referencePosition
     * \param north
     * \param east
     * \param down
     */
    CCoordinateNed(const CCoordinateGeodetic &referencePosition, qreal north, qreal east, qreal down) : CVector3DBase(north, east, down), m_referencePosition(referencePosition), m_hasReferencePosition(true) {}

    /*!
     * \brief Copy constructor
     * \param otherNed
     */
    CCoordinateNed(const CCoordinateNed &otherNed) :
        CVector3DBase(otherNed) , m_hasReferencePosition(otherNed.m_hasReferencePosition), m_referencePosition(otherNed.m_referencePosition) {}

    /*!
     * \brief Equal operator ==
     * \param otherNed
     * \return
     */
    bool operator ==(const CCoordinateNed &otherNed) const
    {
        if (this == &otherNed) return true;
        return this->m_hasReferencePosition == otherNed.m_hasReferencePosition &&
               this->m_referencePosition == otherNed.m_referencePosition &&
               CVector3DBase::operator== (otherNed);
    }

    /*!
     * \brief Unequal operator !=
     * \param otherNed
     * \return
     */
    bool operator !=(const CCoordinateNed &otherNed) const
    {
        if (this == &otherNed) return false;
        return !((*this) == otherNed);
    }

    /*!
     * \brief Assigment operator =
     * \param otherNed
     * \return
     */
    CCoordinateNed &operator =(const CCoordinateNed &otherNed)
    {
        if (this == &otherNed)  return *this; // Same object?
        CVector3DBase::operator = (otherNed);
        this->m_hasReferencePosition = otherNed.m_hasReferencePosition;
        this->m_referencePosition = otherNed.m_referencePosition;
        return (*this);
    }

    /*!
     * \brief Corresponding reference position
     * \return
     */
    CCoordinateGeodetic referencePosition() const
    {
        return this->m_referencePosition;
    }

    /*!
     * \brief Corresponding reference position
     * \return
     */
    bool hasReferencePosition() const
    {
        return this->m_hasReferencePosition;
    }

    /*!
     * \brief North
     * \return
     */
    qreal north() const
    {
        return this->m_vector.x();
    }

    /*!
     * \brief East
     * \return
     */
    qreal east() const
    {
        return this->m_vector.y();
    }

    /*!
     * \brief Down
     * \return
     */
    qreal down() const
    {
        return this->m_vector.z();
    }

    /*!
     * \brief Set north
     * \param north
     */
    void setNorth(qreal north)
    {
        this->m_vector.setX(north);
    }

    /*!
     * \brief Set east
     * \param east
     */
    void setEast(qreal east)
    {
        this->m_vector.setY(east);
    }

    /*!
     * \brief Set down
     * \param down
     */
    void setDown(qreal down)
    {
        this->m_vector.setZ(down);
    }

    /*!
     * \brief Corresponding reference position
     * \param referencePosition
     */
    void setReferencePosition(const CCoordinateGeodetic &referencePosition)
    {
        this->m_referencePosition = referencePosition;
        this->m_hasReferencePosition = true;
    }

};

} // namespace
} // namespace


#endif // guard
