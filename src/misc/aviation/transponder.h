// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_AVIATION_TRANSPONDER_H
#define SWIFT_MISC_AVIATION_TRANSPONDER_H

#include "misc/swiftmiscexport.h"
#include "misc/dictionary.h"
#include "misc/metaclass.h"
#include "misc/propertyindexref.h"
#include "misc/valueobject.h"

#include <QHash>
#include <QMetaType>
#include <QString>
#include <QtGlobal>
#include <tuple>

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::aviation, CTransponder)

namespace swift::misc::aviation
{
    //! Transponder
    class SWIFT_MISC_EXPORT CTransponder : public CValueObject<CTransponder>
    {
    public:
        //! Transponder codes
        enum TransponderMode
        {
            StateStandby = 0, //!< not a real mode, more a state
            ModeMil1 = 1,
            ModeMil2 = 2,
            ModeMil3 = 3,
            ModeMil4 = 4,
            ModeMil5 = 5, //!< military modes
            StateIdent = 10, //!< not a real mode, more a state
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

        //! \copydoc swift::misc::CValueObject::registerMetadata
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

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc swift::misc::mixin::String::toQString
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

        SWIFT_METACLASS(
            CTransponder,
            SWIFT_METAMEMBER(transponderCode),
            SWIFT_METAMEMBER(transponderMode));
    };
} // ns

Q_DECLARE_METATYPE(swift::misc::aviation::CTransponder)
Q_DECLARE_METATYPE(swift::misc::aviation::CTransponder::TransponderMode)

#endif // guard
