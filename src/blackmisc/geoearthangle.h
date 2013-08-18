/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_GEOEARTHANGLE_H
#define BLACKMISC_GEOEARTHANGLE_H
#include "blackmisc/pqangle.h"

namespace BlackMisc
{
namespace Geo
{
/*!
 * \brief Base class for latitude / longitude
 */
template <class LATorLON> class CEarthAngle : public BlackMisc::PhysicalQuantities::CAngle
{
    /*!
     * \brief Unmarshalling operator >>, DBus to object
     * \param argument
     * \param uc
     * \return
     */
    friend const QDBusArgument &operator>>(const QDBusArgument &argument, LATorLON &uc)
    {
        // If I do not have the method here, DBus metasystem tries to stream against
        // a container: inline const QDBusArgument &operator>>(const QDBusArgument &arg, Container<T> &list)
        // Once someone solves this, this methods should go and the
        // CBaseStreamStringifier signature should be used
        CBaseStreamStringifier &sf = uc;
        return argument >> sf;
    }

    /*!
     * \brief Marshalling operator <<, object to DBus
     * \param argument
     * \param pq
     * \return
     */
    friend QDBusArgument &operator<<(QDBusArgument &argument, const LATorLON &uc)
    {
        const CBaseStreamStringifier &sf = uc;
        return argument << sf;
    }

protected:
    /*!
     * \brief Default constructor
     */
    CEarthAngle() : CAngle(0.0, BlackMisc::PhysicalQuantities::CAngleUnit::deg()) {}

    /*!
     * \brief Copy constructor
     * \param latOrLon
     */
    CEarthAngle(const LATorLON &latOrLon) : CAngle(latOrLon) {}

    /*!
     * \brief Init by double value
     * \param value
     * \param unit
     */
    CEarthAngle(double value, const BlackMisc::PhysicalQuantities::CAngleUnit &unit) : CAngle(value, unit) {}

    /*!
     * \brief Init by double value
     * \param angle
     */
    CEarthAngle(const BlackMisc::PhysicalQuantities::CAngle &angle) : CAngle(angle) {}

    /*!
     * \brief String for converter and streaming
     * \return
     */
    virtual QString convertToQString() const
    {
        return this->valueRoundedWithUnit(6);
    }

    /*!
     * \brief Stream to DBus <<
     * \param argument
     */
    virtual void marshallToDbus(QDBusArgument &argument) const
    {
        this->CAngle::marshallToDbus(argument);
    }

    /*!
     * \brief Stream from DBus >>
     * \param argument
     */
    virtual void unmarshallFromDbus(const QDBusArgument &argument)
    {
        this->CAngle::unmarshallFromDbus(argument);
    }

public:
    /*!
     * \brief Virtual destructor
     */
    virtual ~CEarthAngle() {}

    /*!
     * \brief Equal operator ==
     * \param latOrLon
     * \return
     */
    bool operator==(const LATorLON &latOrLon) const
    {
        return this->CAngle::operator ==(latOrLon);
    }

    /*!
     * \brief Not equal operator !=
     * \param latOrLon
     * \return
     */
    bool operator!=(const LATorLON &latOrLon) const
    {
        return this->CAngle::operator !=(latOrLon);
    }

    /*!
     * \brief Plus operator +=
     * \param latOrLon
     * \return
     */
    CEarthAngle &operator +=(const CEarthAngle &latOrLon)
    {
        this->CAngle::operator +=(latOrLon);
        return *this;
    }

    /*!
     * \brief Minus operator-=
     * \param latOrLon
     * \return
     */
    CEarthAngle &operator -=(const CEarthAngle &latOrLon)
    {
        this->CAngle::operator -=(latOrLon);
        return *this;
    }

    /*!
     * \brief Greater operator >
     * \param latOrLon
     * \return
     */
    bool operator >(const LATorLON &latOrLon) const
    {
        return this->CAngle::operator >(latOrLon);
    }

    /*!
     * \brief Less operator <
     * \param latOrLon
     * \return
     */
    bool operator <(const LATorLON &latOrLon) const
    {
        return this->CAngle::operator >(latOrLon);
    }

    /*!
     * \brief Less equal operator <=
     * \param latOrLon
     * \return
     */
    bool operator <=(const LATorLON &latOrLon) const
    {
        return this->CAngle::operator <=(latOrLon);
    }

    /*!
     * \brief Greater equal operator >=
     * \param latOrLon
     * \return
     */
    bool operator >=(const LATorLON &latOrLon) const
    {
        return this->CAngle::operator >=(latOrLon);
    }

    /*!
     * \brief Assignment operator =
     * \param latOrLon
     * \return
     */
    CEarthAngle &operator =(const LATorLON &latOrLon)
    {
        this->CAngle::operator =(latOrLon);
        return *this;
    }

    /*!
     * \brief Plus operator +
     * \param latOrLon
     * \return
     */
    LATorLON operator +(const LATorLON &latOrLon) const
    {
        LATorLON l(*this);
        l += latOrLon;
        return l;
    }

    /*!
     * \brief Minus operator -
     * \param latOrLon
     * \return
     */
    LATorLON operator -(const LATorLON &latOrLon) const
    {
        LATorLON l(*this);
        l -= latOrLon;
        return l;
    }

    /*!
     * Register metadata
     */
    static void registerMetadata();

    /*!
     * \brief Latitude / Longitude from a WGS string such as
     * \param wgsCoordinate 50° 2′ 0″ N / 8° 34′ 14″ E
     * \return
     */
    static LATorLON fromWgs84(const QString &wgsCoordinate);
};

} // namespace
} // namespace

#endif // guard
