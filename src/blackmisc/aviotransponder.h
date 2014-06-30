/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_AVIOTRANSPONDER_H
#define BLACKMISC_AVIOTRANSPONDER_H
#include "blackmisc/aviobase.h"
#include <stdexcept>

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
            //! \brief Transponder codes
            enum TransponderMode
            {
                StateStandby = 0, // not a real mode, more a state
                ModeMil1 = 1, ModeMil2 = 2, ModeMil3 = 3, ModeMil4 = 4, ModeMil5 = 5,
                StateIdent = 10, // not a real mode, more a state
                ModeA = 11,
                ModeC = 12,
                ModeS = 20
            };

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CTransponder)
            qint32 m_transponderCode; //<! Transponder code
            TransponderMode m_transponderMode; //<! Transponder mode

        protected:
            //! \brief Default value?
            virtual bool isDefaultValue() const { return this->m_transponderCode == 0; }

            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n = false) const override;

            //! \copydoc CValueObject::marshallFromDbus()
            virtual void marshallToDbus(QDBusArgument &argument) const override;

            //! \copydoc CValueObject::unmarshallFromDbus()
            virtual void unmarshallFromDbus(const QDBusArgument &argument) override;

            //! \copydoc CValueObject::compareImpl
            virtual int compareImpl(const CValueObject &other) const override;

        public:
            //! \brief Default constructor
            CTransponder() : CAvionicsBase("transponder"), m_transponderCode(0), m_transponderMode(StateStandby) {}

            //! \brief Copy constructor
            CTransponder(const CTransponder &other) : CAvionicsBase(other.getName()),
                m_transponderCode(other.m_transponderCode), m_transponderMode(other.m_transponderMode) {}

            //! \brief Constructor
            CTransponder(const QString &name, qint32 transponderCode, TransponderMode transponderMode) :
                CAvionicsBase(name), m_transponderCode(transponderCode), m_transponderMode(transponderMode)
            {  }

            //! \brief Constructor with transponder mode as string
            CTransponder(const QString &name, qint32 transponderCode, QString transponderMode) :
                CAvionicsBase(name), m_transponderCode(transponderCode), m_transponderMode(StateStandby)
            {
                this->setModeAsString(transponderMode);
            }

            //! \brief Constructor, code as string
            CTransponder(const QString &name, QString transponderCode, TransponderMode transponderMode) :
                CAvionicsBase(name), m_transponderCode(0), m_transponderMode(transponderMode)
            {
                bool ok = false;
                this->m_transponderCode = transponderCode.toUInt(&ok);
                if (!ok) this->m_transponderCode = -1; // will cause assert / exception
            }

            //! \brief Constructor
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

            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override
            {
                return QVariant::fromValue(*this);
            }

            //! \brief Transponder mode as string
            QString getModeAsString() const
            {
                return CTransponder::modeAsString(this->getTransponderMode());
            }

            //! \brief Transponder mode as string
            void setModeAsString(const QString &mode)
            {
                this->setTransponderMode(CTransponder::modeFromString(mode));
            }

            //! \brief Transponder mode
            TransponderMode getTransponderMode() const
            {
                return this->m_transponderMode;
            }

            //! \brief Transponder mode as string
            static const QString &modeAsString(TransponderMode mode);

            //! \brief Transponder code
            qint32 getTransponderCode() const
            {
                return this->m_transponderCode;
            }

            //! \brief Transponder code
            QString getTransponderCodeFormatted() const;

            //! \brief Formatted with code / mode
            QString getTransponderCodeAndModeFormatted() const;

            //! \brief Set transponder code
            void setTransponderCode(qint32 transponderCode)
            {
                this->m_transponderCode = transponderCode;
            }

            //! \brief Set transponder code
            void setTransponderCode(const QString &transponderCode);

            //! \brief Mode from string
            static TransponderMode modeFromString(const QString &modeString);

            //! \brief Set transponder mode
            void setTransponderMode(TransponderMode mode)
            {
                this->m_transponderMode = mode ;
            }

            //! \brief Set emergency
            void setEmergency()
            {
                this->m_transponderCode = 7700;
            }

            //! \brief Set VFR
            void setVFR()
            {
                this->m_transponderCode = 7000;
            }

            //! \brief Set IFR
            void setIFR()
            {
                this->m_transponderCode = 2000;
            }

            //! \brief operator ==
            bool operator ==(const CTransponder &other) const
            {
                return
                    this->m_transponderCode == other.m_transponderCode &&
                    this->getTransponderMode() == other.getTransponderMode() &&
                    this->CAvionicsBase::operator ==(other);
            }

            //! \brief operator =!
            bool operator !=(const CTransponder &other) const
            {
                return !((*this) == other);
            }

            //! \brief Transponder unit
            static CTransponder getStandardTransponder(qint32 transponderCode, TransponderMode mode)
            {
                return CTransponder("Transponder", transponderCode, mode);
            }

            //! \copydoc CValueObject::getValueHash()
            virtual uint getValueHash() const override;

            //! \brief Is valid transponder code?
            static bool isValidTransponderCode(const QString &transponderCode);

            //! \brief Is valid transponder code?
            static bool isValidTransponderCode(qint32 transponderMode);

            //! \brief Register metadata
            static void registerMetadata();

            //! \copydoc CValueObject::toJson
            virtual QJsonObject toJson() const override;

            //! \copydoc CValueObject::fromJson
            void fromJson(const QJsonObject &json) override;

            //! \copydoc TupleConverter<>::jsonMembers()
            static const QStringList &jsonMembers();
        };
    } // namespace
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Aviation::CTransponder, (o.m_transponderCode, o.m_transponderMode))
Q_DECLARE_METATYPE(BlackMisc::Aviation::CTransponder)

#endif // guard
