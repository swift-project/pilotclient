/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of Swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_CALLSIGN_H
#define BLACKMISC_CALLSIGN_H

#include "propertyindex.h"
#include "icon.h"
#include "blackmiscfreefunctions.h"

namespace BlackMisc
{
    namespace Aviation
    {
        //! Value object encapsulating information of a callsign.
        class CCallsign : public CValueObject<CCallsign>
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

            //! Default constructor.
            CCallsign() = default;

            //! Constructor
            CCallsign(const QString &callsign, const QString &telephonyDesignator = "")
                : m_callsignAsSet(callsign.trimmed()), m_callsign(CCallsign::unifyCallsign(callsign.trimmed())), m_telephonyDesignator(telephonyDesignator.trimmed())
            {}

            //! Constructor, needed to disambiguate implicit conversion from string literal.
            CCallsign(const char *callsign)
                : m_callsignAsSet(callsign), m_callsign(CCallsign::unifyCallsign(callsign))
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

            //! Get callsign.
            const QString &asString() const { return this->m_callsign; }

            //! Get callsign.
            const QString &getStringAsSet() const { return this->m_callsignAsSet; }

            //! Get callsign telephony designator (how callsign is pronounced)
            const QString &getTelephonyDesignator() const { return this->m_telephonyDesignator; }

            //! Get ICAO code, if this makes sense (EDDF_TWR -> EDDF)
            QString getIcaoCode() const { return m_callsign.left(4).toUpper(); }

            //! Makes this callsign looking like an observer callsign (DAMBZ -> DAMBZ_OBS)
            QString getAsObserverCallsignString() const;

            //! Get the callsign suffix ("TWR", "ATIS" ...) if any ("_" is removed)
            QString getSuffix() const;

            //! Suffix such as "_TWR"?
            bool hasSuffix() const;

            //! Equals callsign string?
            bool equalsString(const QString &callsignString) const;

            //! \copydoc CValueObject::toIcon()
            virtual BlackMisc::CIcon toIcon() const override { return convertToIcon(*this); }

            //! \copydoc CValueObject::propertyByIndex
            virtual CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const override;

            //! \copydoc CValueObject::setPropertyByIndex
            virtual void setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index) override;

            //! Valid callsign?
            static bool isValidCallsign(const QString &callsign);

            //! List of real ATC suffixes (e.g. TWR);
            static const QStringList &atcCallsignSuffixes();

            //! List of real ("TWR") and treated like ATC suffixes (e.g. OBS);
            static const QStringList &atcAlikeCallsignSuffixes();

            //! Suffix to icon
            static const BlackMisc::CIcon &suffixToIcon(const QString &suffix);

            //! Representing icon
            static const CIcon &convertToIcon(const CCallsign &callsign);

        protected:
            //! \copydoc CValueObject::convertToQString()
            virtual QString convertToQString(bool i18n = false) const override;

            //! Unify the callsign
            static QString unifyCallsign(const QString &callsign);

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CCallsign)
            QString m_callsignAsSet;
            QString m_callsign;
            QString m_telephonyDesignator;
        };
    } // namespace
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Aviation::CCallsign, (
                                   attr(o.m_callsign, flags<CaseInsensitiveComparison>()),
                                   attr(o.m_callsignAsSet, flags<DisabledForComparison>()),
                                   attr(o.m_telephonyDesignator, flags<DisabledForComparison>())
                               ))
Q_DECLARE_METATYPE(BlackMisc::Aviation::CCallsign)

#endif // guard
