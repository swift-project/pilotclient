#include "avtrack.h"

namespace BlackMisc {

/**
 * Own implementation for streaming
 */
QString CTrack::stringForStreamingOperator() const
{
    QString s = CAngle::stringForStreamingOperator();
    return s.append(this->m_magnetic ? " magnetic" : " true");
}

/**
 * Assigment
 */
CTrack& CTrack::operator =(const CTrack &otherTrack)
{
    // Check for self-assignment!
    if (this == &otherTrack)  return *this;
    CAngle::operator = (otherTrack);
    this->m_magnetic = otherTrack.m_magnetic;
    return (*this);
}

/**
 * Equal?
 */
bool CTrack::operator ==(const CTrack &otherTrack)
{
    return otherTrack.m_magnetic == this->m_magnetic && CAngle::operator ==(otherTrack);
}

/**
 * Unequal?
 */
bool CTrack::operator !=(const CTrack &otherTrack)
{
    return !((*this) == otherTrack);
}

} // namespace
