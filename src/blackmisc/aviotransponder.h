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
        class CTransponder : public CValueObjectStdTuple<CTransponder, CAvionicsBase>
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
            CTransponder() : CValueObjectStdTuple("transponder"), m_transponderCode(0), m_transponderMode(StateStandby) {}

            //! Constructor
            CTransponder(const QString &name, int transponderCode, TransponderMode transponderMode) :
                CValueObjectStdTuple(name), m_transponderCode(transponderCode), m_transponderMode(transponderMode)
            {  }

            //! Constructor with transponder mode as string
            CTransponder(const QString &name, int transponderCode, QString transponderMode) :
                CValueObjectStdTuple(name), m_transponderCode(transponderCode), m_transponderMode(StateStandby)
            {
                this->setModeAsString(transponderMode);
            }

            //! Constructor, code as string
            CTransponder(const QString &name, QString transponderCode, TransponderMode transponderMode) :
                CValueObjectStdTuple(name), m_transponderCode(0), m_transponderMode(transponderMode)
            {
                bool ok = false;
                this->m_transponderCode = transponderCode.toUInt(&ok);
                if (!ok) this->m_transponderCode = -1; // will cause assert / exception
            }

            //! Constructor
            CTransponder(const QString &name, QString transponderCode, QString transponderMode) :
                CValueObjectStdTuple(name), m_transponderCode(0), m_transponderMode(StateStandby)
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
                return modeAsString(this->getTransponderMode());
            }

            //! In any sending mode such as MIL1 oder ModeS
            bool isInNormalSendingMode() const;

            //! Standby?
            bool isInStandby() const { return StateStandby == m_transponderMode; }

            //! Standby?
            bool isIdentifying() const { return StateIdent == m_transponderMode; }

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
            int getTransponderCode() const
            {
                return this->m_transponderCode;
            }

            //! Transponder code
            QString getTransponderCodeFormatted() const;

            //! Formatted with code / mode
            QString getTransponderCodeAndModeFormatted() const;

            //! Set transponder code
            void setTransponderCode(int transponderCode) { this->m_transponderCode = transponderCode; }

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

            //! Transponder unit
            static CTransponder getStandardTransponder(qint32 transponderCode, TransponderMode mode)
            {
                return CTransponder("Transponder", transponderCode, mode);
            }

            //! \copydoc CValueObject::propertyByIndex
            virtual CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const override;

            //! \copydoc CValueObject::setPropertyByIndex
            virtual void setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index) override;

            //! Is valid transponder code?
            static bool isValidTransponderCode(const QString &transponderCode);

            //! Is valid transponder code?
            static bool isValidTransponderCode(qint32 transponderMode);

        protected:
            //! Default value?
            virtual bool isDefaultValue() const { return this->m_transponderCode == 0; }

            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n = false) const override;

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CTransponder)
            int m_transponderCode;             //!< Transponder code
            TransponderMode m_transponderMode; //!< Transponder mode
        };

    }
}

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Aviation::CTransponder, (o.m_transponderCode, o.m_transponderMode))
Q_DECLARE_METATYPE(BlackMisc::Aviation::CTransponder)

#endif // guard
