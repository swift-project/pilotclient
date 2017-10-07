/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of Swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_CALLSIGN_H
#define BLACKMISC_AVIATION_CALLSIGN_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/icon.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/metaclassprivate.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/variant.h"

#include <QMetaType>
#include <QString>
#include <tuple>

class QStringList;

namespace BlackMisc
{
    namespace Aviation
    {
        //! Value object encapsulating information of a callsign.
        class BLACKMISC_EXPORT CCallsign : public CValueObject<CCallsign>
        {
        public:
            //! Indexes
            enum ColumnIndex
            {
                IndexCallsignString = BlackMisc::CPropertyIndex::GlobalIndexCCallsign,
                IndexCallsignStringAsSet,
                IndexTelephonyDesignator,
                IndexSuffix
            };

            //! Representing what
            enum TypeHint
            {
                NoHint,
                Aircraft,
                Atc
            };

            //! Default constructor.
            CCallsign() {}

            //! Constructor
            CCallsign(const QString &callsign, TypeHint hint = NoHint)
                : m_callsignAsSet(callsign.trimmed()), m_callsign(CCallsign::unifyCallsign(callsign)), m_typeHint(hint)
            {}

            //! Constructor
            CCallsign(const QString &callsign, const QString &telephonyDesignator, TypeHint hint = NoHint)
                : m_callsignAsSet(callsign.trimmed()), m_callsign(CCallsign::unifyCallsign(callsign)), m_telephonyDesignator(telephonyDesignator.trimmed()), m_typeHint(hint)
            {}

            //! Constructor, needed to disambiguate implicit conversion from string literal.
            CCallsign(const char *callsign, TypeHint hint = NoHint)
                : m_callsignAsSet(callsign), m_callsign(CCallsign::unifyCallsign(callsign)), m_typeHint(hint)
            {}

            //! Is empty?
            bool isEmpty() const { return this->m_callsignAsSet.isEmpty(); }

            //! ATC callsign
            //! \sa atcCallsignSuffixes()
            bool isAtcCallsign() const;

            //! ATC alike callsign
            //! \sa atcAlikeCallsignSuffixes()
            bool isAtcAlikeCallsign() const;

            //! Observer callsign?
            bool isObserverCallsign() const;

            //! Supervisor?
            bool isSupervisorCallsign() const;

            //! Get callsign (normalized)
            const QString &asString() const { return this->m_callsign; }

            //! Get callsign.
            const QString &getStringAsSet() const { return this->m_callsignAsSet; }

            //! Get callsign telephony designator (how callsign is pronounced)
            const QString &getTelephonyDesignator() const { return this->m_telephonyDesignator; }

            //! Type hint
            TypeHint getTypeHint() const { return m_typeHint; }

            //! Type hint
            void setTypeHint(TypeHint hint) { this->m_typeHint = hint; }

            //! Get ICAO code, if this makes sense (EDDF_TWR -> EDDF)
            QString getIcaoCode() const;

            //! Makes this callsign looking like an observer callsign (DAMBZ -> DAMBZ_OBS)
            QString getAsObserverCallsignString() const;

            //! Get the callsign suffix ("TWR", "ATIS" ...) if any ("_" is removed)
            QString getSuffix() const;

            //! Airline suffix (e.g. DLH1234 -> DLH) if applicable
            QString getAirlineSuffix() const;

            //! Suffix such as "_TWR"?
            bool hasSuffix() const;

            //! Equals callsign string?
            bool equalsString(const QString &callsignString) const;

            //! Valid callsign?
            bool isValid() const;

            //! Valid callsign?
            static bool isValidAircraftCallsign(const QString &callsign);

            //! Valid callsign?
            static bool isValidAircraftCallsign(const CCallsign &callsign);

            //! Valid callsign?
            static bool isValidAtcCallsign(const QString &callsign);

            //! Valid callsign?
            static bool isValidAtcCallsign(const CCallsign &callsign);

            //! Unify the callsign by removing illegal characters
            static QString unifyCallsign(const QString &callsign);

            //! List of real ATC suffixes (e.g. TWR);
            static const QStringList &atcCallsignSuffixes();

            //! List of real ("TWR") and treated like ATC suffixes (e.g. OBS);
            static const QStringList &atcAlikeCallsignSuffixes();

            //! Suffix to icon
            static const BlackMisc::CIcon &atcSuffixToIcon(const QString &suffix);

            //! Representing icon
            static const CIcon &convertToIcon(const CCallsign &callsign);

            //! \copydoc BlackMisc::Mixin::Icon::toIcon()
            BlackMisc::CIcon toIcon() const { return convertToIcon(*this); }

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(const BlackMisc::CPropertyIndex &index, const CVariant &variant);

            //! Compare for index
            int comparePropertyByIndex(const CPropertyIndex &index, const CCallsign &compareValue) const;

            //! \copydoc BlackMisc::Mixin::String::toQString()
            QString convertToQString(bool i18n = false) const;

        private:
            QString  m_callsignAsSet;
            QString  m_callsign;
            QString  m_telephonyDesignator;
            TypeHint m_typeHint = NoHint;

            BLACK_METACLASS(
                CCallsign,
                BLACK_METAMEMBER(callsign, 0, CaseInsensitiveComparison),
                BLACK_METAMEMBER(callsignAsSet, 0, DisabledForComparison | DisabledForHashing),
                BLACK_METAMEMBER(telephonyDesignator, 0, DisabledForComparison | DisabledForHashing),
                BLACK_METAMEMBER(typeHint, 0, DisabledForComparison | DisabledForHashing)
            );
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CCallsign)

#endif // guard
