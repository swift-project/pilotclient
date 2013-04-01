#include "avaltitude.h"

namespace BlackMisc {

/**
 * Own implementation for streaming
 */
QString CAltitude::stringForStreamingOperator() const
{
    QString s = CLength::stringForStreamingOperator();
    return s.append(this->m_msl ? " MSL" : " AGL");
}

/**
 * Assigment
 */
CAltitude& CAltitude::operator =(const CAltitude &otherAltitude)
{
    // Check for self-assignment!
    if (this == &otherAltitude)  return *this;
    CLength::operator = (otherAltitude);
    this->m_msl = otherAltitude.m_msl;
    return (*this);
}

/**
 * Equal?
 */
bool CAltitude::operator ==(const CAltitude &otherAltitude)
{
    return otherAltitude.m_msl == this->m_msl && CLength::operator ==(otherAltitude);
}

/**
 * Unequal?
 */
bool CAltitude::operator !=(const CAltitude &otherAltitude)
{
    return !((*this) == otherAltitude);
}

} // namespace
