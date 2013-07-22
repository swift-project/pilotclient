/*  Copyright (C) 2013 VATSIM Community / authors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_AVIONAVSYSTEM_H
#define BLACKMISC_AVIONAVSYSTEM_H
#include "blackmisc/aviomodulator.h"

namespace BlackMisc
{
namespace Aviation
{

/*!
 * \brief NAV system (radio navigation)
 */
class CNavSystem : public CModulator<CNavSystem>
{
private:
    /*!
     * \brief Valid civil aviation frequency?
     * \param f
     * \return
     */
    bool isValidCivilNavigationFrequency(BlackMisc::PhysicalQuantities::CFrequency f) const
    {
        double fr = f.valueRounded(BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz(), this->m_digits);
        return fr >= 108.0 && fr <= 117.95;
    }
    /*!
     * \brief Valid military aviation frequency?
     * \param f
     * \return
     */
    bool isValidMilitaryNavigationFrequency(BlackMisc::PhysicalQuantities::CFrequency f) const
    {
        double fr = f.valueRounded(BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz(), this->m_digits);
        return fr >= 960.0 && fr <= 1215.0; // valid TACAN frequency
    }
    /*!
     * \brief Constructor
     * \param validate
     * \param name
     * \param activeFrequency
     * \param standbyFrequency
     * \param digits
     *
     */
    CNavSystem(bool validate, const QString &name, const BlackMisc::PhysicalQuantities::CFrequency &activeFrequency, const BlackMisc::PhysicalQuantities::CFrequency &standbyFrequency, int digits = 3):
        CModulator(name, activeFrequency, standbyFrequency, digits)
    {
        this->validate(validate);
    }

protected:
    /*!
     * \brief Are the set values valid / in range?
     * \return
     */
    bool validValues() const
    {
        if (this->isDefaultValue()) return true; // special case
        bool v =
            (this->isValidCivilNavigationFrequency(this->getFrequencyActive()) ||
             this->isValidMilitaryNavigationFrequency(this->getFrequencyActive())) &&
            (this->isValidCivilNavigationFrequency(this->getFrequencyStandby()) ||
             this->isValidMilitaryNavigationFrequency(this->getFrequencyStandby()));
        return v;
    }
    /*!
     * \brief Validate values by assert and exception
     * \param strict
     * \throws std::range_error
     * \remarks Cannot be virtualsince already used in constructor
     * \return
     */
    bool validate(bool strict = true) const
    {
        if (this->isDefaultValue()) return true;
        bool valid = this->validValues();
        if (!strict) return valid;
        Q_ASSERT_X(valid, "CModulator::validate", "illegal values");
        if (!valid) throw std::range_error("Illegal values in CModulator::validate");
        return true;
    }
public:
    /*!
     * Default constructor
     */
    CNavSystem() : CModulator() {}
    /*!
     * \brief Copy constructor
     * \param otherSystem
     */
    CNavSystem(const CNavSystem &otherSystem) : CModulator(otherSystem) {}
    /*!
     * \brief Constructor
     * \param name
     * \param activeFrequency
     * \param standbyFrequency
     * \param digits
     */
    CNavSystem(const QString &name, const BlackMisc::PhysicalQuantities::CFrequency &activeFrequency, const BlackMisc::PhysicalQuantities::CFrequency &standbyFrequency, int digits = 3):
        CModulator(name, activeFrequency, standbyFrequency, digits)
    {
        this->validate(true);
    }
    /*!
     * \brief Set active frequency
     * \param frequencyMHz
     */
    void setFrequencyActiveMHz(double frequencyMHz)
    {
        CModulator::setFrequencyActiveMHz(frequencyMHz);
        this->validate(true);
    }
    /*!
     * \brief Set standby frequency
     * \param frequencyMHz
     */
    void setFrequencyStandbyMHz(double frequencyMHz)
    {
        CModulator::setFrequencyStandbyMHz(frequencyMHz);
        this->validate(true);
    }
    /*!
     * \brief Assigment operator =
     * \param otherSystem
     * \return
     */
    CNavSystem &operator =(const CNavSystem &otherSystem)
    {
        CModulator::operator =(otherSystem);
        return (*this);
    }
    /*!
     * \brief operator ==
     * \param otherSystem
     * \return
     */
    bool operator ==(const CNavSystem &otherSystem) const
    {
        return CModulator::operator ==(otherSystem);
    }
    /*!
     * \brief operator ==
     * \param otherSystem
     * \return
     */
    bool operator !=(const CNavSystem &otherSystem) const
    {
        return CModulator::operator !=(otherSystem);
    }

    /*!
     * Try to get a NAV unit with given name and frequency. Returns true in case an object
     * has been sucessfully created,otherwise returns a default object.
     * \param navSystem
     * \param name
     * \param activeFrequencyMHz
     * \param standbyFrequencyMHz
     * \return
     */
    static bool tryGetNavSystem(CNavSystem &navSystem, const QString &name, double activeFrequencyMHz, double standbyFrequencyMHz = -1)
    {
        navSystem = CNavSystem(false, name, BlackMisc::PhysicalQuantities::CFrequency(activeFrequencyMHz, BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz()), BlackMisc::PhysicalQuantities::CFrequency(standbyFrequencyMHz < 0 ? activeFrequencyMHz : standbyFrequencyMHz, BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz()));
        bool s;
        if (!(s = navSystem.validate(false))) navSystem = CNavSystem(); // reset to default
        return s;
    }
    /*!
     * Try to get a NAV unit with given name and frequency. Returns true in case an object
     * has been sucessfully created, otherwise returns a default object.
     * \param navSystem
     * \param name
     * \param activeFrequency
     * \param standbyFrequency
     * \return
     */
    static bool tryGetNavSystem(CNavSystem &navSystem, const QString &name, BlackMisc::PhysicalQuantities::CFrequency activeFrequency, BlackMisc::PhysicalQuantities::CFrequency standbyFrequency = CModulator::FrequencyNotSet())
    {
        navSystem = CNavSystem(false, name, activeFrequency, standbyFrequency);
        bool s;
        if (!(s = navSystem.validate(false))) navSystem = CNavSystem(); // reset to default
        return s;
    }
    /*!
     * \brief NAV1 unit
     * \param activeFrequencyMHz
     * \param standbyFrequencyMHz
     * \return
     */
    static CNavSystem getNav1System(double activeFrequencyMHz, double standbyFrequencyMHz = -1)
    {
        return CNavSystem(CModulator::NameNav1(), BlackMisc::PhysicalQuantities::CFrequency(activeFrequencyMHz, BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz()), BlackMisc::PhysicalQuantities::CFrequency(standbyFrequencyMHz < 0 ? activeFrequencyMHz : standbyFrequencyMHz, BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz()));
    }
    /*!
     * \brief NAV1 unit
     * \param activeFrequency
     * \param standbyFrequency
     * \return
     */
    static CNavSystem getNav1System(BlackMisc::PhysicalQuantities::CFrequency activeFrequency, BlackMisc::PhysicalQuantities::CFrequency standbyFrequency = CModulator::FrequencyNotSet())
    {
        return CNavSystem(CModulator::NameNav1(), activeFrequency, standbyFrequency ==  CModulator::FrequencyNotSet() ? activeFrequency : standbyFrequency);
    }
    /*!
     * \brief Try to get NAV unit
     * \param navSystem
     * \param activeFrequencyMHz
     * \param standbyFrequencyMHz
     * \return
     */
    static bool tryGetNav1System(CNavSystem &navSystem, double activeFrequencyMHz, double standbyFrequencyMHz = -1)
    {
        return CNavSystem::tryGetNavSystem(navSystem, CModulator::NameNav1(), activeFrequencyMHz, standbyFrequencyMHz);
    }
    /*!
     * \brief Try to get NAV unit
     * \param navSystem
     * \param activeFrequency
     * \param standbyFrequency
     * \return
     */
    static bool tryGetNav1System(CNavSystem &navSystem, BlackMisc::PhysicalQuantities::CFrequency activeFrequency, BlackMisc::PhysicalQuantities::CFrequency standbyFrequency = CModulator::FrequencyNotSet())
    {
        return CNavSystem::tryGetNavSystem(navSystem, CModulator::NameNav1(), activeFrequency, standbyFrequency);
    }
    /*!
     * \brief NAV2 unit
     * \param activeFrequencyMHz
     * \param standbyFrequencyMHz
     * \return
     */
    static CNavSystem getNav2System(double activeFrequencyMHz, double standbyFrequencyMHz = -1)
    {
        return CNavSystem(CModulator::NameNav2(), BlackMisc::PhysicalQuantities::CFrequency(activeFrequencyMHz, BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz()), BlackMisc::PhysicalQuantities::CFrequency(standbyFrequencyMHz < 0 ? activeFrequencyMHz : standbyFrequencyMHz, BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz()));
    }
    /*!
     * \brief NAV2 unit
     * \param activeFrequency
     * \param standbyFrequency
     * \return
     */
    static CNavSystem getNav2System(BlackMisc::PhysicalQuantities::CFrequency activeFrequency, BlackMisc::PhysicalQuantities::CFrequency standbyFrequency = CModulator::FrequencyNotSet())
    {
        return CNavSystem(CModulator::NameNav2(), activeFrequency, standbyFrequency ==  CModulator::FrequencyNotSet() ? activeFrequency : standbyFrequency);
    }
    /*!
     * \brief Try to get NAV unit
     * \param navSystem
     * \param activeFrequencyMHz
     * \param standbyFrequencyMHz
     * \return
     */
    static bool tryGetNav2System(CNavSystem &navSystem, double activeFrequencyMHz, double standbyFrequencyMHz = -1)
    {
        return CNavSystem::tryGetNavSystem(navSystem, CModulator::NameNav2(), activeFrequencyMHz, standbyFrequencyMHz);
    }
    /*!
     * \brief Try to get NAV unit
     * \param navSystem
     * \param activeFrequency
     * \param standbyFrequency
     * \return
     */
    static bool tryGetNav2System(CNavSystem &navSystem, BlackMisc::PhysicalQuantities::CFrequency activeFrequency, BlackMisc::PhysicalQuantities::CFrequency standbyFrequency = CModulator::FrequencyNotSet())
    {
        return CNavSystem::tryGetNavSystem(navSystem, CModulator::NameNav2(), activeFrequency, standbyFrequency);
    }
};

} // namespace
} // namespace
Q_DECLARE_METATYPE(BlackMisc::Aviation::CNavSystem)

#endif // BLACKMISC_AVIONAVSYSTEM_H
