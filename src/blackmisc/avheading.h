#ifndef AVHEADING_H
#define AVHEADING_H
#include "blackmisc/pqangle.h"

namespace BlackMisc {

/*!
 * \brief Heading as used in aviation, can be true or magnetic heading
 * \remarks Intentionally allowing +/- CAngle , and >= / <= CAngle.
 */
class CHeading : public CAngle
{
private:
    bool m_magnetic; //!< magnetic or true heading?

protected:
    /*!
     * \brief Specific stream operation for heading
     * \return
     */
    virtual QString stringForStreamingOperator() const;

public:
    /*!
     * \brief Default constructor: 0 heading true
     */
    CHeading() : CAngle(0, CAngleUnit::rad()), m_magnetic(true) {}
    /*!
     * \brief Constructor
     * \param value
     * \param magnetic
     * \param unit
     */
    CHeading(double value, bool magnetic, const CAngleUnit &unit) : CAngle(value, unit), m_magnetic(magnetic) {}
    /*!
     * \brief Constructor
     * \param value
     * \param magnetic
     * \param unit
     */
    CHeading(int value, bool magnetic, const CAngleUnit &unit) : CAngle(value, unit), m_magnetic(magnetic) {}
    /*!
     * \brief Constructor by CAngle
     * \param heading
     * \param magnetic
     */
    CHeading(CAngle heading, bool magnetic) : CAngle(), m_magnetic(magnetic) {
        CAngle::operator =(heading);
    }
    /*!
     * \brief Copy constructor
     * \param otherHeading
     */
    CHeading(const CHeading &otherHeading) : CAngle(otherHeading), m_magnetic(otherHeading.m_magnetic) {}
    /*!
     * \brief Assignment operator =
     * \param otherQuantity
     * @return
     */
    CHeading &operator =(const CHeading &otherHeading);
    /*!
     * \brief Equal operator ==
     * \param otherQuantity
     * @return
     */
    bool operator ==(const CHeading &otherHeading);
    /*!
     * \brief Unequal operator ==
     * \param otherQuantity
     * @return
     */
    bool operator !=(const CHeading &otherHeading);
    /*!
     * \brief Magnetic heading?
     * \return
     */
    bool isMagneticHeading() const { return this->m_magnetic; }
    /*!
     * \brief True heading?
     * \return
     */
    bool isTrueHeading() const { return !this->m_magnetic; }
};

} // namespace
#endif // AVHEADING_H
