/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_TRANSPONDER_H
#define BLACKMISC_AVIATION_TRANSPONDER_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/dictionary.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/propertyindexref.h"
#include "blackmisc/valueobject.h"

#include <QHash>
#include <QMetaType>
#include <QString>
#include <QtGlobal>
#include <tuple>

BLACK_DECLARE_VALUEOBJECT_MIXINS(BlackMisc::Aviation, CTransponder)

namespace BlackMisc::Aviation
{
    //! Transponder
    class BLACKMISC_EXPORT CTransponder : public CValueObject<CTransponder>
    {
    public:
        //! Transponder codes
        enum TransponderMode
        {
            StateStandby = 0, //!< not a real mode, more a state
            ModeMil1 = 1, ModeMil2 = 2, ModeMil3 = 3, ModeMil4 = 4, ModeMil5 = 5, //!< military modes
            StateIdent = 10,  //!< not a real mode, more a state
            ModeA = 11,
            ModeC = 12,
            ModeS = 20
        };

        //! Indexes
        enum ColumnIndex
        {
            IndexMode = CPropertyIndexRef::GlobalIndexCTransponder,
            IndexModeAsString,
            IndexTransponderCode,
            IndexTransponderCodeFormatted,
            IndexTransponderCodeAndModeFormatted
        };

        //! \copydoc BlackMisc::CValueObject::registerMetadata
        static void registerMetadata();

        //! Default constructor
        CTransponder() : m_transponderCode(0), m_transponderMode(StateStandby) {}

        //! Constructor
        CTransponder(int transponderCode, TransponderMode transponderMode);

        //! Constructor with transponder mode as string
        CTransponder(int transponderCode, const QString &transponderMode);

        //! Constructor, code as string
        CTransponder(const QString &transponderCode, TransponderMode transponderMode);

        //! Constructor
        CTransponder(const QString &transponderCode, const QString &transponderMode);

        //! Are set values valid?
        bool validValues() const;

        //! Transponder mode as string
        QString getModeAsString() const { return modeAsString(this->getTransponderMode()); }

        //! Transponder mode as short string
        QString getModeAsShortString() const { return modeAsShortString(this->getTransponderMode()); }

        //! In any sending mode such as MIL1 oder ModeS
        bool isInNormalSendingMode() const;

        //! Standby?
        bool isInStandby() const { return StateStandby == this->getTransponderMode(); }

        //! Standby?
        bool isIdentifying() const { return StateIdent == this->getTransponderMode(); }

        //! Transponder mode as string
        void setModeAsString(const QString &mode) { this->setTransponderMode(CTransponder::modeFromString(mode)); }

        //! Transponder mode
        TransponderMode getTransponderMode() const { return static_cast<TransponderMode>(m_transponderMode); }

        //! Transponder mode toggled
        void toggleTransponderMode();

        //! Transponder mode as string
        static const QString &modeAsString(TransponderMode mode);

        //! Transponder mode as short string
        static const QString &modeAsShortString(CTransponder::TransponderMode mode);

        //! Transponder code
        int getTransponderCode() const { return m_transponderCode; }

        //! Transponder code
        QString getTransponderCodeFormatted() const;

        //! Formatted with code / mode
        QString getTransponderCodeAndModeFormatted() const;

        //! Set transponder code
        void setTransponderCode(int transponderCode) { m_transponderCode = transponderCode; }

        //! Set transponder code
        void setTransponderCode(const QString &transponderCode);

        //! Mode from string
        static TransponderMode modeFromString(const QString &modeString);

        //! Set transponder mode
        bool setTransponderMode(TransponderMode mode);

        //! Set emergency
        void setEmergency() { m_transponderCode = 7700; }

        //! Set VFR
        void setVFR() { m_transponderCode = 7000; }

        //! Set IFR
        void setIFR() { m_transponderCode = 2000; }

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        QVariant propertyByIndex(CPropertyIndexRef index) const;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! Is valid transponder code?
        static bool isValidTransponderCode(const QString &transponderCode);

        //! Is valid transponder code?
        static bool isValidTransponderCode(qint32 transponderMode);

        //! Transponder unit
        static CTransponder getStandardTransponder(qint32 transponderCode, TransponderMode mode);

    private:
        //! Default value?
        bool isDefaultValue() const { return m_transponderCode == 0; }

        int m_transponderCode; //!< Transponder code
        int m_transponderMode; //!< Transponder mode

        BLACK_METACLASS(
            CTransponder,
            BLACK_METAMEMBER(transponderCode),
            BLACK_METAMEMBER(transponderMode)
        );
    };
} // ns

Q_DECLARE_METATYPE(BlackMisc::Aviation::CTransponder)
Q_DECLARE_METATYPE(BlackMisc::Aviation::CTransponder::TransponderMode)

#endif // guard
