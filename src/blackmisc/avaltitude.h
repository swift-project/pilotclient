/*  Copyright (C) 2013 VATSIM Community / contributors
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
public:
    /*!
     * Enum type to distinguish between MSL and AGL
     */
    enum ReferenceDatum
    {
        MeanSeaLevel = 0,   //!< MSL
        AboveGround = 1,    //!< AGL
    };

private:
    ReferenceDatum m_datum; //!< MSL or AGL?

protected:
    /*!
     * \brief Specific stream operation for Altitude
     * \param i18n
     * \return
     */
    virtual QString convertToQString(bool i18n) const;

    /*!
     * \brief Stream to DBus <<
     * \param argument
     */
    virtual void marshallToDbus(QDBusArgument &argument) const
    {
        this->CLength::marshallToDbus(argument);
        argument << qint32(this->m_datum);
    }

    /*!
     * \brief Stream from DBus >>
     * \param argument
     */
    virtual void unmarshallFromDbus(const QDBusArgument &argument)
    {
        this->CLength::unmarshallFromDbus(argument);
        qint32 datum;
        argument >> datum;
        this->m_datum = static_cast<ReferenceDatum>(datum);
    }

public:
    /*!
     * \brief Default constructor: 0 Altitude true
     */
    CAltitude() : BlackMisc::PhysicalQuantities::CLength(0, BlackMisc::PhysicalQuantities::CLengthUnit::m()), m_datum(MeanSeaLevel) {}

    /*!
     * \brief Constructor
     * \param value
     * \param datum MSL or AGL?
     * \param unit
     */
    CAltitude(double value, ReferenceDatum datum, const BlackMisc::PhysicalQuantities::CLengthUnit &unit) : BlackMisc::PhysicalQuantities::CLength(value, unit), m_datum(datum) {}

    /*!
     * \brief Constructor by CLength
     * \param altitude
     * \param datum
     */
    CAltitude(BlackMisc::PhysicalQuantities::CLength altitude, ReferenceDatum datum) : BlackMisc::PhysicalQuantities::CLength(altitude), m_datum(datum) {}

    /*!
     * \brief Equal operator ==
     * \param other
     * @return
     */
    bool operator ==(const CAltitude &other);

    /*!
     * \brief Unequal operator ==
     * \param other
     * @return
     */
    bool operator !=(const CAltitude &other);

    /*!
     * \brief AGL Above ground level?
     * \return
     */
    bool isAboveGroundLevel() const
    {
        return AboveGround == this->m_datum;
    }

    /*!
     * \brief MSL Mean sea level?
     * \return
     */
    bool isMeanSeaLevel() const
    {
        return MeanSeaLevel == this->m_datum;
    }

    /*!
     * \brief Get reference datum (MSL or AGL)
     * \return
     */
    ReferenceDatum getReferenceDatum() const { return m_datum; }

    /*!
     * \brief Register metadata
     */
    static void registerMetadata();
};

} // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAltitude)

#endif // guard
