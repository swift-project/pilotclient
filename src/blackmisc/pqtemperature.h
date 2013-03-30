#ifndef CTEMPERATURE_H
#define CTEMPERATURE_H
#include "pqphysicalquantity.h"

namespace BlackMisc {

/*!
 * \brief Physical unit temperature
 * \author KWB
 */
class CTemperature : public CPhysicalQuantity<CTemperatureUnit,CTemperature>
{
public:
    /*!
     * \brief Default constructor
     */
    CTemperature() : CPhysicalQuantity(0, CTemperatureUnit::K(), CTemperatureUnit::K()) {}
    /**
     *\brief Copy constructor
     */
    CTemperature(const CTemperature &temperature) : CPhysicalQuantity(temperature){}
    /*!
     * \brief Init by int value
     * \param value
     * \param unit
     */
    CTemperature(qint32 value, const CTemperatureUnit &unit): CPhysicalQuantity(value, unit, CTemperatureUnit::K()) {}
    /*!
     *\brief Init by double value
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

#endif // CTEMPERATURE_H
