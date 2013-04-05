#include "blackmisc/aviomodulator.h"
#include "blackmisc/aviocomsystem.h"

namespace BlackMisc {

/**
 * Toggle standby <-> active
 */
template <class AVIO> void CModulator<AVIO>::toggleActiveStandby()
{
    CFrequency a = this->m_frequencyActive;
    this->m_frequencyActive = this->m_frequencyStandby;
    this->m_frequencyStandby = a;
}

/**
 * String representation
 */
template <class AVIO> QString CModulator<AVIO>::stringForStreamingOperator() const
{
    QString s(this->m_name);
    s.append(" Active: ").append(this->m_frequencyActive.unitValueRoundedWithUnit(3));
    s.append(" Standby: ").append(this->m_frequencyStandby.unitValueRoundedWithUnit(3));
    return s;
}

/**
 * Assigment operator =
 */
template <class AVIO> CModulator<AVIO>& CModulator<AVIO>::operator=(const CModulator<AVIO> &otherModulator) {

    if (this == &otherModulator)  return *this; // Same object?
    this->m_frequencyActive = otherModulator.m_frequencyActive;
    this->m_frequencyStandby = otherModulator.m_frequencyStandby;
    this->m_name = otherModulator.m_name;
    this->m_digits = otherModulator.m_digits;
    return *this;
}

/**
 * Equal operator ==
 */
template <class AVIO> bool CModulator<AVIO>::operator ==(const CModulator<AVIO> &otherModulator) const
{
    if(this == &otherModulator) return true;
    return (this->m_name == otherModulator.m_name &&
            this->m_frequencyActive == otherModulator.m_frequencyActive &&
            this->m_frequencyStandby == otherModulator.m_frequencyStandby);
}

/**
 * Equal operator !=
 */
template <class AVIO> bool CModulator<AVIO>::operator !=(const CModulator<AVIO> &otherModulator) const
{
    return !(otherModulator == (*this));
}


// see here for the reason of thess forward instantiations
// http://www.parashift.com/c++-faq/separate-template-class-defn-from-decl.html
template class CModulator<CComSystem>;

} // namespace
