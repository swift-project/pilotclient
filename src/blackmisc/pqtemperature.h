/*  Copyright (C) 2013 VATSIM Community
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_CTEMPERATURE_H
#define BLACKMISC_CTEMPERATURE_H
#include "pqphysicalquantity.h"

namespace BlackMisc
{
namespace PhysicalQuantities
{

/*!
 * \brief Physical unit temperature
 * \author KWB
 */
class CTemperature : public CPhysicalQuantity<CTemperatureUnit, CTemperature>
{
public:
    /*!
     * \brief Default constructor
     */
    CTemperature() : CPhysicalQuantity(0, CTemperatureUnit::K(), CTemperatureUnit::K()) {}

    /**
     * \brief Copy constructor from base type
     */
    CTemperature(const CPhysicalQuantity &base) : CPhysicalQuantity(base) {}

    /*!
     * \brief Init by int value
     * \param value
     * \param unit
     */
    CTemperature(qint32 value, const CTemperatureUnit &unit): CPhysicalQuantity(value, unit, CTemperatureUnit::K()) {}

    /*!
     * \brief Init by double value
     * \param value
     * \param unit
     */
    CTemperature(double value, const CTemperatureUnit &unit): CPhysicalQuantity(value, unit, CTemperatureUnit::K()) {}

    /*!
     * \brief Destructor
     */
    virtual ~CTemperature() {}
};

} // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CTemperature)

#endif // BLACKMISC_CTEMPERATURE_H
