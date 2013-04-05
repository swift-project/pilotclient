#ifndef AVIOCOMUNIT_H
#define AVIOCOMUNIT_H
#include "blackmisc/aviomodulator.h"

namespace BlackMisc {

/*!
 * \brief COM system (aka "radio")
 */
class CComSystem : public CModulator<CComSystem>
{
private:
    /*!
     * \brief Valid civil aviation frequency?
     * \param f
     * \return
     */
    bool isValidCivilAviationFrequency(CFrequency f) { double fr = f.valueRounded(CFrequencyUnit::MHz(), this->m_digits); return fr >= 118.0 && fr <= 136.975; }
    /*!
     * \brief Valid military aviation frequency?
     * \param f
     * \return
     */
    bool isValidMilitaryFrequency(CFrequency f) { double fr = f.valueRounded(CFrequencyUnit::MHz(), this->m_digits); return fr >= 220.0 && fr <= 399.95; }

public:
    /*!
     * Default constructor
     */
    CComSystem() : CModulator() {}
    /*!
     * \brief Copy constructor
     * \param otherUnit
     */
    CComSystem(const CComSystem &otherUnit) : CModulator(otherUnit) {}
    /*!
     * \brief Constructor
     * \param name
     * \param activeFrequency
     * \param standbyFrequency
     * \param digits
     */
    CComSystem(const QString &name, const CFrequency &activeFrequency, const CFrequency &standbyFrequency, int digits =3):
        CModulator(name, activeFrequency, standbyFrequency, digits) {}
    /*!
     * \brief Set active frequency
     * \param frequencyMHz
     */
    void setFrequencyActiveMHz(double frequencyMHz) { CModulator::setFrequencyActiveMHz(frequencyMHz); }
    /*!
     * \brief Set standby frequency
     * \param frequencyMHz
     */
    void setFrequencyStandbyMHz(double frequencyMHz) { CModulator::setFrequencyStandbyMHz(frequencyMHz); }
    /*!
     * \brief Set UNICOM frequency as active
     */
    void setActiveUnicom() { this->toggleActiveStandby(); this->setFrequencyActive(CPhysicalQuantitiesConstants::FrequencyUnicom());}
    /*!
     * \brief Set International Air Distress 121.5MHz
     */
    void setActiveInternationalAirDistress() { this->toggleActiveStandby(); this->setFrequencyActive(CPhysicalQuantitiesConstants::FrequencyInternationalAirDistress());}
    /*!
     * \brief Assigment operator =
     * \param otherSystem
     * \return
     */
    CComSystem& operator =(const CComSystem &otherSystem) { CModulator::operator =(otherSystem); return (*this); }
    /*!
     * \brief operator ==
     * \param otherSystem
     * \return
     */
    bool operator ==(const CComSystem &otherSystem) const  { return CModulator::operator ==(otherSystem); }
    /*!
     * \brief operator ==
     * \param otherSystem
     * \return
     */
    bool operator !=(const CComSystem &otherSystem) const  { return CModulator::operator !=(otherSystem); }

    /*!
     * \brief Are the set values valid / in range?
     * \return
     */
    virtual bool validValues() {
        return
                this->isValidCivilAviationFrequency(this->getFrequencyActive()) &&
                this->isValidMilitaryFrequency(this->getFrequencyActive()) &&
                this->isValidCivilAviationFrequency(this->getFrequencyStandby()) &&
                this->isValidMilitaryFrequency(this->getFrequencyStandby());
    }
    /*!
     * \brief COM1 unit
     * \param activeFrequencyMHz
     * \param standbyFrequencyMHz
     * \return
     */
    static CComSystem getCom1Unit(double activeFrequencyMHz, double standbyFrequencyMHz = -1) {
        return CComSystem(CModulator::NameCom1(), CFrequency(activeFrequencyMHz, CFrequencyUnit::MHz()), CFrequency(standbyFrequencyMHz < 0 ? activeFrequencyMHz : standbyFrequencyMHz,CFrequencyUnit::MHz()));}
    /*!
     * \brief COM1 unit
     * \param activeFrequency
     * \param standbyFrequency
     * \return
     */
    static CComSystem getCom1Unit(CFrequency activeFrequency, CFrequency standbyFrequency = CModulator::FrequencyNotSet()) {
        return CComSystem(CModulator::NameCom1(), activeFrequency, standbyFrequency ==  CModulator::FrequencyNotSet() ? activeFrequency : standbyFrequency);}
    /*!
     * \brief COM2 unit
     * \param activeFrequencyMHz
     * \param standbyFrequencyMHz
     * \return
     */
    static CComSystem getCom2Unit(double activeFrequencyMHz, double standbyFrequencyMHz = -1) {
        return CComSystem(CModulator::NameCom2(), CFrequency(activeFrequencyMHz, CFrequencyUnit::MHz()), CFrequency(standbyFrequencyMHz < 0 ? activeFrequencyMHz : standbyFrequencyMHz,CFrequencyUnit::MHz()));}
    /*!
     * \brief COM2 unit
     * \param activeFrequency
     * \param standbyFrequency
     * \return
     */
    static CComSystem getCom2Unit(CFrequency activeFrequency, CFrequency standbyFrequency = CModulator::FrequencyNotSet()) {
        return CComSystem(CModulator::NameCom2(), activeFrequency, standbyFrequency ==  CModulator::FrequencyNotSet() ? activeFrequency : standbyFrequency);}
    /*!
     * \brief COM3 unit
     * \param activeFrequencyMHz
     * \param standbyFrequencyMHz
     * \return
     */
    static CComSystem getCom3Unit(double activeFrequencyMHz, double standbyFrequencyMHz = -1) {
        return CComSystem(CModulator::NameCom3(), CFrequency(activeFrequencyMHz, CFrequencyUnit::MHz()), CFrequency(standbyFrequencyMHz < 0 ? activeFrequencyMHz : standbyFrequencyMHz,CFrequencyUnit::MHz()));}
    /*!
     * \brief COM3 unit
     * \param activeFrequency
     * \param standbyFrequency
     * \return
     */
    static CComSystem getCom3Unit(CFrequency activeFrequency, CFrequency standbyFrequency = CModulator::FrequencyNotSet()) {
        return CComSystem(CModulator::NameCom3(), activeFrequency, standbyFrequency ==  CModulator::FrequencyNotSet() ? activeFrequency : standbyFrequency);}

};

} // namespace

#endif // AVIOCOMUNIT_H
