#ifndef AVALTITUDE_H
#define AVALTITUDE_H

#include "blackmisc/pqlength.h"

namespace BlackMisc {

/*!
 * \brief Altitude as used in aviation, can be AGL or MSL Altitude
 * \remarks Intentionally allowing +/- CLength , and >= / <= CLength.
 */
class CAltitude : public CLength
{
private:
    bool m_msl; //!< MSL or AGL?

protected:
    /*!
     * \brief Specific stream operation for Altitude
     * \return
     */
    virtual QString stringForStreamingOperator() const;

public:
    /*!
     * \brief Default constructor: 0 Altitude true
     */
    CAltitude() : CLength(0, CLengthUnit::m()), m_msl(true) {}
    /*!
     * \brief Constructor
     * \param value
     * \param msl MSL or AGL?
     * \param unit
     */
    CAltitude(double value, bool msl, const CLengthUnit &unit) : CLength(value, unit), m_msl(msl) {}
    /*!
     * \brief Constructor
     * \param value
     * \param msl MSL or AGL?
     * \param unit
     */
    CAltitude(int value, bool msl, const CLengthUnit &unit) : CLength(value, unit), m_msl(msl) {}
    /*!
     * \brief Constructor by CLength
     * \param Altitude
     * \param msl
     */
    CAltitude(CLength altitude, bool msl) : CLength(), m_msl(msl) {
        CLength::operator =(altitude);
    }
    /*!
     * \brief Copy constructor
     * \param otherAltitude
     */
    CAltitude(const CAltitude &otherAltitude) : CLength(otherAltitude), m_msl(otherAltitude.m_msl) {}
    /*!
     * \brief Assignment operator =
     * \param otherQuantity
     * @return
     */
    CAltitude &operator =(const CAltitude &otherAltitude);
    /*!
     * \brief Equal operator ==
     * \param otherQuantity
     * @return
     */
    bool operator ==(const CAltitude &otherAltitude);
    /*!
     * \brief Unequal operator ==
     * \param otherQuantity
     * @return
     */
    bool operator !=(const CAltitude &otherAltitude);
    /*!
     * \brief AGL Above ground level?
     * \return
     */
    bool isAboveGroundLevel() const { return !this->m_msl; }
    /*!
     * \brief MSL Mean sea level?
     * \return
     */
    bool isMeanSeaLevel() const { return this->m_msl; }
};

} // namespace

#endif // AVALTITUDE_H
