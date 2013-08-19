/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_AVHEADING_H
#define BLACKMISC_AVHEADING_H
#include "blackmisc/pqangle.h"

namespace BlackMisc
{
namespace Aviation
{

/*!
 * \brief Heading as used in aviation, can be true or magnetic heading
 * \remarks Intentionally allowing +/- CAngle , and >= / <= CAngle.
 */
class CHeading : public BlackMisc::PhysicalQuantities::CAngle
{
public:
    /*!
     * Enum type to distinguish between true north and magnetic north
     */
    enum ReferenceNorth
    {
        Magnetic = 0,   //!< magnetic north
        True = 1,       //!< true north
    };

private:
    ReferenceNorth m_north; //!< magnetic or true?

protected:
    /*!
     * \brief Specific stream operation for heading
     * \param i18n
     * \return
     */
    virtual QString convertToQString(bool i18n = false) const;

    /*!
     * \brief Stream to DBus <<
     * \param argument
     */
    virtual void marshallToDbus(QDBusArgument &argument) const
    {
        this->CAngle::marshallToDbus(argument);
        argument << qint32(this->m_north);
    }

    /*!
     * \brief Stream from DBus >>
     * \param argument
     */
    virtual void unmarshallFromDbus(const QDBusArgument &argument)
    {
        this->CAngle::unmarshallFromDbus(argument);
        qint32 north;
        argument >> north;
        this->m_north = static_cast<ReferenceNorth>(north);
    }

public:
    /*!
     * \brief Default constructor: 0 heading true
     */
    CHeading() : CAngle(0, BlackMisc::PhysicalQuantities::CAngleUnit::rad()), m_north(Magnetic) {}

    /*!
     * \brief Constructor
     * \param value
     * \param north
     * \param unit
     */
    CHeading(double value, ReferenceNorth north, const BlackMisc::PhysicalQuantities::CAngleUnit &unit) : CAngle(value, unit), m_north(north) {}

    /*!
     * \brief Constructor by CAngle
     * \param north
     * \param magnetic
     */
    CHeading(CAngle heading, ReferenceNorth north) : CAngle(heading), m_north(north) {}

    /*!
     * \brief Equal operator ==
     * \param other
     * @return
     */
    bool operator ==(const CHeading &other);

    /*!
     * \brief Unequal operator ==
     * \param other
     * @return
     */
    bool operator !=(const CHeading &other);

    /*!
     * \brief Magnetic heading?
     * \return
     */
    bool isMagneticHeading() const { return Magnetic == this->m_north; }

    /*!
     * \brief True heading?
     * \return
     */
    bool isTrueHeading() const { return True == this->m_north; }

    /*!
     * \brief Get reference north (magnetic or true)
     * \return
     */
    ReferenceNorth getReferenceNorth() const { return m_north; }

    /*!
     * \brief Register metadata
     */
    static void registerMetadata();
};

} // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CHeading)

#endif // guard
