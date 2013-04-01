#ifndef AVLATERALPOSITION_H
#define AVLATERALPOSITION_H
#include "blackmisc/avaltitude.h"
#include "blackmisc/pqconstants.h"

namespace BlackMisc {

/*!
 * \brief Vertical (Z) positions of an aircraft
 */
class CAviationVerticalPositions
{
    /*!
     * \brief Stream << overload to be used in debugging messages
     * \param d
     * \param unit
     * \return
     */
    friend QDebug operator<<(QDebug d, const CAviationVerticalPositions &positions);

    /*!
     * Stream operator for log messages
     * \brief operator <<
     * \param log
     * \param unit
     * \return
     */
    friend CLogMessage operator<<(CLogMessage log, const CAviationVerticalPositions &positions);

private:
    CLength m_height;  //!< height
    CLength m_elevation;  //!< elevation
    CAltitude m_altitude;  //!< altitude

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
    CAviationVerticalPositions(const CAltitude &altitude, const CLength &elevation, const CLength &height);
    /*!
     * \brief Copy constructor
     * \param otherPosition
     */
    CAviationVerticalPositions(const CAviationVerticalPositions &otherPosition);
    /*!
     * \brief Assignment operator =
     * \param otherQuantity
     * @return
     */
    CAviationVerticalPositions &operator =(const CAviationVerticalPositions &otherPositions);
    /*!
     * \brief Equal operator ==
     * \param otherQuantity
     * @return
     */
    bool operator ==(const CAviationVerticalPositions &otherPositions);
    /*!
     * \brief Unequal operator ==
     * \param otherQuantity
     * @return
     */
    bool operator !=(const CAviationVerticalPositions &otherPositions);
    /*!
     * \brief Height
     * \return
     */
    CLength getHeight() const { return this->m_height;}
    /*!
     * \brief Elevation
     * \return
     */
    CLength getElevation() const { return this->m_elevation;}
    /*!
     * \brief Altitude
     * \return
     */
    CAltitude getAltitude()const { return this->m_altitude; }
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

#endif // AVLATERALPOSITION_H
