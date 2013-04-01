#include "avheading.h"

namespace BlackMisc {

/**
 * Own implementation for streaming
 */
QString CHeading::stringForStreamingOperator() const
{
    QString s = CAngle::stringForStreamingOperator();
    return s.append(this->m_magnetic ? " magnetic" : " true");
}

/**
 * Assigment
 */
CHeading& CHeading::operator =(const CHeading &otherHeading)
{
    // Check for self-assignment!
    if (this == &otherHeading)  return *this;
    CAngle::operator = (otherHeading);
    this->m_magnetic = otherHeading.m_magnetic;
    return (*this);
}

/**
 * Equal?
 */
bool CHeading::operator ==(const CHeading &otherHeading)
{
    return otherHeading.m_magnetic == this->m_magnetic && CAngle::operator ==(otherHeading);
}

/**
 * Unequal?
 */
bool CHeading::operator !=(const CHeading &otherHeading)
{
    return !((*this) == otherHeading);
}

} // namespace
