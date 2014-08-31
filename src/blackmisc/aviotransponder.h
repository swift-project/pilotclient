/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIOTRANSPONDER_H
#define BLACKMISC_AVIOTRANSPONDER_H
#include "blackmisc/aviobase.h"

namespace BlackMisc
{
    namespace Aviation
    {
        /*!
         * Transponder
         */
        class CTransponder : public CAvionicsBase
        {
        public:
            //! Transponder codes
            enum TransponderMode
            {
                StateStandby = 0, // not a real mode, more a state
                ModeMil1 = 1, ModeMil2 = 2, ModeMil3 = 3, ModeMil4 = 4, ModeMil5 = 5,
                StateIdent = 10, // not a real mode, more a state
                ModeA = 11,
                ModeC = 12,
                ModeS = 20
            };

            //! Indexes
            enum ColumnIndex
            {
                IndexMode = BlackMisc::CPropertyIndex::GlobalIndexCTransponder,
                IndexModeAsString,
                IndexTransponderCode,
                IndexTransponderCodeFormatted,
                IndexTransponderCodeAndModeFormatted
            };

            //! Default constructor
            CTransponder() : CAvionicsBase("transponder"), m_transponderCode(0), m_transponderMode(StateStandby) {}

            //! Copy constructor
            CTransponder(const CTransponder &other) : CAvionicsBase(other.getName()),
                m_transponderCode(other.m_transponderCode), m_transponderMode(other.m_transponderMode) {}

            //! Constructor
            CTransponder(const QString &name, qint32 transponderCode, TransponderMode transponderMode) :
                CAvionicsBase(name), m_transponderCode(transponderCode), m_transponderMode(transponderMode)
            {  }

            //! Constructor with transponder mode as string
            CTransponder(const QString &name, qint32 transponderCode, QString transponderMode) :
                CAvionicsBase(name), m_transponderCode(transponderCode), m_transponderMode(StateStandby)
            {
                this->setModeAsString(transponderMode);
            }

            //! Constructor, code as string
            CTransponder(const QString &name, QString transponderCode, TransponderMode transponderMode) :
                CAvionicsBase(name), m_transponderCode(0), m_transponderMode(transponderMode)
            {
                bool ok = false;
                this->m_transponderCode = transponderCode.toUInt(&ok);
                if (!ok) this->m_transponderCode = -1; // will cause assert / exception
            }

            //! Constructor
            CTransponder(const QString &name, QString transponderCode, QString transponderMode) :
                CAvionicsBase(name), m_transponderCode(0), m_transponderMode(StateStandby)
            {
                bool ok = false;
                this->m_transponderCode = transponderCode.toUInt(&ok);
                if (!ok) this->m_transponderCode = -1; // will cause assert / exception
                this->setModeAsString(transponderMode);
            }

            //! \copydoc CAvionicsBase::validValues
            virtual bool validValues() const override;

            //! Transponder mode as string
            QString getModeAsString() const
            {
                return CTransponder::modeAsString(this->getTransponderMode());
            }

            //! Transponder mode as string
            void setModeAsString(const QString &mode)
            {
                this->setTransponderMode(CTransponder::modeFromString(mode));
            }

            //! Transponder mode
            TransponderMode getTransponderMode() const
            {
                return this->m_transponderMode;
            }

            //! Transponder mode as string
            static const QString &modeAsString(TransponderMode mode);

            //! Transponder code
            qint32 getTransponderCode() const
            {
                return this->m_transponderCode;
            }

            //! Transponder code
            QString getTransponderCodeFormatted() const;

            //! Formatted with code / mode
            QString getTransponderCodeAndModeFormatted() const;

            //! Set transponder code
            void setTransponderCode(qint32 transponderCode) { this->m_transponderCode = transponderCode; }

            //! Set transponder code
            void setTransponderCode(const QString &transponderCode);

            //! Mode from string
            static TransponderMode modeFromString(const QString &modeString);

            //! Set transponder mode
            void setTransponderMode(TransponderMode mode) { this->m_transponderMode = mode ; }

            //! Set emergency
            void setEmergency() { this->m_transponderCode = 7700; }

            //! Set VFR
            void setVFR() { this->m_transponderCode = 7000; }

            //! Set IFR
            void setIFR() { this->m_transponderCode = 2000; }

            //! operator ==
            bool operator ==(const CTransponder &other) const
            {
                return
                    this->m_transponderCode == other.m_transponderCode &&
                    this->getTransponderMode() == other.getTransponderMode() &&
                    this->CAvionicsBase::operator ==(other);
            }

            //! operator !=
            bool operator !=(const CTransponder &other) const { return !((*this) == other); }

            //! Transponder unit
            static CTransponder getStandardTransponder(qint32 transponderCode, TransponderMode mode)
            {
                return CTransponder("Transponder", transponderCode, mode);
            }

            //! \copydoc CValueObject::getValueHash()
            virtual uint getValueHash() const override;

            //! \copydoc CValueObject::toJson
            virtual QJsonObject toJson() const override;

            //! \copydoc CValueObject::convertFromJson
            virtual void convertFromJson(const QJsonObject &json) override;

            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override  { return QVariant::fromValue(*this); }

            //! \copydoc CValueObject::convertFromQVariant
            virtual void convertFromQVariant(const QVariant &variant) override { BlackMisc::setFromQVariant(this, variant); }

            //! \copydoc CValueObject::propertyByIndex
            virtual QVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const override;

            //! \copydoc CValueObject::setPropertyByIndex(variant, index)
            virtual void setPropertyByIndex(const QVariant &variant, const BlackMisc::CPropertyIndex &index) override;

            //! JSON member names
            static const QStringList &jsonMembers();

            //! Is valid transponder code?
            static bool isValidTransponderCode(const QString &transponderCode);

            //! Is valid transponder code?
            static bool isValidTransponderCode(qint32 transponderMode);

            //! Register metadata
            static void registerMetadata();

        protected:
            //! Default value?
            virtual bool isDefaultValue() const { return this->m_transponderCode == 0; }

            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n = false) const override;

            //! \copydoc CValueObject::marshallFromDbus()
            virtual void marshallToDbus(QDBusArgument &argument) const override;

            //! \copydoc CValueObject::unmarshallFromDbus()
            virtual void unmarshallFromDbus(const QDBusArgument &argument) override;

            //! \copydoc CValueObject::compareImpl
            virtual int compareImpl(const CValueObject &other) const override;

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CTransponder)
            qint32 m_transponderCode;          //!< Transponder code
            TransponderMode m_transponderMode; //!< Transponder mode

        };
    } // namespace
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Aviation::CTransponder, (o.m_transponderCode, o.m_transponderMode))
Q_DECLARE_METATYPE(BlackMisc::Aviation::CTransponder)

#endif // guard
