/*  Copyright (C) 2013 VATSIM Community / authors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_AVIOTRANSPONDER_H
#define BLACKMISC_AVIOTRANSPONDER_H
#include "blackmisc/aviobase.h"

namespace BlackMisc
{
namespace Aviation
{

/*!
 * \brief Transponder
 */
class CTransponder : public CAvionicsBase
{
public:
    /*!
     * \brief Our transponder codes
     */
    enum TransponderMode {
        StateStandby = 0, // not a real mode, more a state
        ModeMil1 = 1, ModeMil2 = 2, ModeMil3 = 3, ModeMil4 = 4, ModeMil5 = 5,
        StateIdent = 10, // not a real mode, more a state
        ModeA = 11,
        ModeC = 12,
        ModeS = 20
    };
private:
    qint32 m_transponderCode; //<! Transponder code
    TransponderMode m_transponderMode; //<! Transponder mode

    /*!
     * \brief Constructor for validation
     * \param validate
     * \param name
     * \param transponderCode
     * \param transponderMode
     */
    CTransponder(bool validate, const QString &name, qint32 transponderCode, TransponderMode transponderMode) :
        m_transponderCode(transponderCode), m_transponderMode(transponderMode), CAvionicsBase(name) {
        this->validate(validate);
    }
    /*!
     * \brief Constructor for validation
     * \param validate
     * \param name
     * \param transponderCode
     * \param transponderMode
     */
    CTransponder(bool validate, const QString &name, const QString transponderCode, TransponderMode transponderMode) :
        m_transponderCode(0), m_transponderMode(transponderMode), CAvionicsBase(name) {
        bool ok = false;
        this->m_transponderCode = transponderCode.toUInt(&ok);
        if (!ok)this->m_transponderCode = -1; // will cause assert / exception
        this->validate(validate);
    }
protected:
    /*!
     * \brief Are the set values valid / in range?
     * \return
     */
    bool validValues() const;
    /*!
     * \brief Default value
     * \return
     */
    virtual bool isDefaultValue() const {
        return this->m_transponderCode == 0;
    }
    /*!
     * \brief Validate values by assert and exception
     * \param strict
     * \throws std::range_error
     * \remarks Cannot be virtualsince already used in constructor
     * \return
     */
    bool validate(bool strict = true) const;
    /*!
     * \brief Meaningful string representation
     * \return
     */
    virtual QString stringForStreamingOperator() const;
public:
    /*!
     * Default constructor
     */
    CTransponder() : CAvionicsBase("default"), m_transponderCode(0), m_transponderMode(StateStandby) {}
    /*!
     * \brief Copy constructor
     * \param otherTransponder
     */
    CTransponder(const CTransponder &otherTransponder) : CAvionicsBase(otherTransponder.getName()),
        m_transponderCode(otherTransponder.m_transponderCode), m_transponderMode(otherTransponder.m_transponderMode) {}
    /*!
     * \brief Constructor
     * \param name
     * \param transponderCode
     * \param transponderMode
     */
    CTransponder(const QString &name, qint32 transponderCode, TransponderMode transponderMode) :
        CAvionicsBase(name), m_transponderCode(transponderCode), m_transponderMode(transponderMode) {
        this->validate(true);
    }
    /*!
     * \brief Constructor
     * \param name
     * \param transponderCode
     * \param transponderMode
     */
    CTransponder(const QString &name, const QString &transponderCode, TransponderMode transponderMode) :
        CAvionicsBase(name), m_transponderCode(0), m_transponderMode(transponderMode) {
        bool ok = false;
        this->m_transponderCode = transponderCode.toUInt(&ok);
        if (!ok)this->m_transponderCode = -1; // will cause assert / exception
        this->validate(true);
    }
    /*!
     * \brief Transponder mode as string
     * \return
     * \throws std::range_erros
     */
    QString getModeAsString() const;
    /*!
     * \brief Transponder mode
     * \return
     */
    TransponderMode getTransponderMode() const {
        return this->m_transponderMode;
    }
    /*!
     * \brief Transponder code
     * \return
     */
    qint32 getTransponderCode() const {
        return this->m_transponderCode;
    }
    /*!
     * \brief Transponder code
     * \return
     */
    QString getTransponderCodeFormatted() const;
    /*!
     * \brief Set transponder code
     * \param transponderCode
     */
    void setTransponderCode(qint32 transponderCode) {
        this->m_transponderCode = transponderCode;
        this->validate(true);
    }
    /*!
     * \brief Set transponder mode
     * \param mode
     */
    void setTransponderMode(TransponderMode mode) {
        this->m_transponderMode = mode ;
        this->validate(true);
    }
    /*!
     * \brief Set emergency
     */
    void setEmergency() {
        this->m_transponderCode = 7700;
    }
    /*!
     * \brief Set emergency
     */
    void setVFR() {
        this->m_transponderCode = 7000;
    }
    /*!
     * \brief Set emergency
     */
    void setIFR() {
        this->m_transponderCode = 2000;
    }
    /*!
     * \brief Assigment operator =
     * \param otherTransponder
     * \return
     */
    CTransponder &operator =(const CTransponder &otherTransponder) {
        CAvionicsBase::operator =(otherTransponder);
        this->m_transponderMode = otherTransponder.m_transponderMode;
        this->m_transponderCode = otherTransponder.m_transponderCode;
        return (*this);
    }
    /*!
     * \brief operator ==
     * \param otherTransponder
     * \return
     */
    bool operator ==(const CTransponder &otherTransponder) const  {
        return
            this->m_transponderCode == otherTransponder.m_transponderCode &&
            this->m_transponderMode == otherTransponder.m_transponderMode &&
            CAvionicsBase::operator ==(otherTransponder);
    }
    /*!
     * \brief operator =!
     * \param otherSystem
     * \return
     */
    bool operator !=(const CTransponder &otherSystem) const  {
        return !((*this) == otherSystem);
    }

    /*!
     * Try to get a Transponder unit with given name and code. Returns true in case an object
     * has been sucessfully created, otherwise returns a default object.
     * \param transponder
     * \param name
     * \param transponderCode
     * \param mode
     * \return
     */
    static bool tryGetTransponder(CTransponder &transponder, const QString &name, qint32 transponderCode, TransponderMode mode) {
        transponder = CTransponder(false, name, transponderCode, mode);
        bool s;
        if (!(s = transponder.validate(false))) transponder = CTransponder(); // reset to default
        return s;
    }
    /*!
     * Try to get a Transponder unit with given name and code. Returns true in case an object
     * has been sucessfully created, otherwise returns a default object.
     * \param transponder
     * \param name
     * \param transponderCode
     * \param mode
     * \return
     */
    static bool tryGetTransponder(CTransponder &transponder, const QString &name, const QString &transponderCode, TransponderMode mode) {
        transponder = CTransponder(false, name, transponderCode, mode);
        bool s;
        if (!(s = transponder.validate(false))) transponder = CTransponder(); // reset to default
        return s;
    }
    /*!
     * \brief Transponder unit
     * \param transponderCode
     * \param mode
     * \return
     */
    static CTransponder getStandardTransponder(qint32 transponderCode, TransponderMode mode) {
        return CTransponder("Transponder", transponderCode, mode);
    }
    /*!
     * \brief Try to get Transponder unit
     * \param transponder
     * \param transponderCode
     * \param mode
     * \return
     */
    static bool tryGetStandardTransponder(CTransponder &transponder, qint32 transponderCode, TransponderMode mode) {
        return CTransponder::tryGetTransponder(transponder, "Transponder", transponderCode, mode);
    }

    /*!
     * \brief Try to get Transponder unit
     * \param transponder
     * \param transponderCode
     * \param mode
     * \return
     */
    static bool tryGetStandardTransponder(CTransponder &transponder, const QString &transponderCode, TransponderMode mode) {
        return CTransponder::tryGetTransponder(transponder, "Transponder", transponderCode, mode);
    }

};

} // namespace
} // namespace

#endif // BLACKMISC_AVIOTRANSPONDER_H
