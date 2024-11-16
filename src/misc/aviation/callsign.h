// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_AVIATION_CALLSIGN_H
#define SWIFT_MISC_AVIATION_CALLSIGN_H

#include <tuple>

#include <QMetaType>
#include <QString>

#include "misc/metaclass.h"
#include "misc/propertyindexref.h"
#include "misc/statusmessage.h"
#include "misc/swiftmiscexport.h"
#include "misc/valueobject.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::aviation, CCallsign)

namespace swift::misc
{
    class CStatusMessageList;

    namespace aviation
    {
        //! Value object encapsulating information of a callsign.
        class SWIFT_MISC_EXPORT CCallsign : public CValueObject<CCallsign>
        {
        public:
            //! Indexes
            enum ColumnIndex
            {
                IndexCallsignString = CPropertyIndexRef::GlobalIndexCCallsign,
                IndexCallsignStringAsSet,
                IndexTelephonyDesignator,
                IndexSuffix,
                IndexSuffixSortOrder
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
            CCallsign(const QString &callsign, TypeHint hint = NoHint);

            //! Constructor
            CCallsign(const QString &callsign, const QString &telephonyDesignator, TypeHint hint = NoHint);

            //! Constructor, needed to disambiguate implicit conversion from string literal.
            CCallsign(const char *callsign, TypeHint hint = NoHint);

            //! Is empty?
            bool isEmpty() const { return m_callsignAsSet.isEmpty(); }

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

            //! Pseudo callsing for broadcast messages
            //! \remark hack, workaround for VATSIM using "*" as callsign for text messages
            bool isBroadcastCallsign() const;

            //! Set a human readable name as "broadcast" callsign
            //! \remark hack, workaround for VATSIM using "*" as callsign for text messages
            void markAsBroadcastCallsign();

            //! Set a human readable name as "wallop-channel" callsign
            void markAsWallopCallsign();

            //! Returns true if this is a co-pilot callsign of pilot. The logic is that the callsign is the same as the
            //! pilot one but with a single character as suffix. e.g Pilot logged in as DLH123, observer logged in as
            //! DLH123A
            bool isMaybeCopilotCallsign(const CCallsign &pilotCallsign) const;

            //! Get callsign (normalized)
            const QString &asString() const { return m_callsign; }

            //! Get callsign.
            const QString &getStringAsSet() const { return m_callsignAsSet; }

            //! The callsign string used with FSD
            QString getFsdCallsignString() const;

            //! Same as set callsign?
            bool isSameAsSet() const;

            //! Get callsign telephony designator (how callsign is pronounced)
            const QString &getTelephonyDesignator() const { return m_telephonyDesignator; }

            //! Type hint
            TypeHint getTypeHint() const { return m_typeHint; }

            //! Type hint
            void setTypeHint(TypeHint hint) { m_typeHint = hint; }

            //! Get ICAO code, if this makes sense (EDDF_TWR -> EDDF)
            QString getIcaoCode() const;

            //! Makes this callsign looking like an observer callsign (DAMBZ -> DAMBZ_OBS)
            QString getAsObserverCallsignString() const;

            //! Get the callsign suffix ("TWR", "ATIS" ...) if any ("_" is removed)
            QString getSuffix() const;

            //! @{
            //! Airline suffix (e.g. DLH1234 -> DLH) if applicable
            QString getAirlinePrefix() const;
            QString getAirlinePrefix(QString &flightNumber) const;
            QString getAirlinePrefix(QString &flightNumber, QString &flightIdentification) const;
            //! @}

            //! Flight number (e.g. DLH1234 -> 1234) if applicable
            QString getFlightIndentification() const;

            //! Flight number (e.g. DLH1234 -> 1234) if applicable
            QString getFlightNumber() const;

            //! Flight number as integer
            int getFlightNumberInt() const;

            //! Suffix such as "_TWR"?
            bool hasSuffix() const;

            //! Has an ATC suffix?
            bool hasAtcSuffix() const;

            //! Sort order by suffix
            int getSuffixSortOrder() const;

            //! Equals callsign string?
            bool equalsString(const QString &callsignString) const;

            //! Valid callsign?
            bool isValid() const;

            //! \copydoc swift::misc::mixin::Icon::toIcon()
            CIcons::IconIndex toIcon() const { return convertToIcon(*this).getIndex(); }

            //! \copydoc swift::misc::mixin::Index::propertyByIndex
            QVariant propertyByIndex(CPropertyIndexRef index) const;

            //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
            void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

            //! \copydoc swift::misc::mixin::Index::comparePropertyByIndex
            int comparePropertyByIndex(CPropertyIndexRef index, const CCallsign &compareValue) const;

            //! \copydoc swift::misc::mixin::String::toQString()
            QString convertToQString(bool i18n = false) const;

            //! Clear this callsign
            void clear();

            //! Index for ATC suffix, if unknown int max value
            static int suffixToSortOrder(const QString &suffix);

            //! Valid callsign?
            static bool isValidAircraftCallsign(const QString &callsign);

            //! Valid callsign?
            static bool isValidAircraftCallsign(const CCallsign &callsign);

            //! Valid callsign?
            static bool isValidAtcCallsign(const QString &callsign);

            //! Valid callsign?
            static bool isValidAtcCallsign(const CCallsign &callsign);

            //! Unify the callsign by removing illegal characters
            static QString unifyCallsign(const QString &callsign, TypeHint hint = NoHint);

            //! List of real ATC suffixes (e.g. TWR);
            static const QStringList &atcCallsignSuffixes();

            //! List of real ("TWR") and treated like ATC suffixes (e.g. OBS);
            static const QStringList &atcAlikeCallsignSuffixes();

            //! Does this look like an ATC callsign
            static bool looksLikeAtcCallsign(const QString &callsign);

            //! Suffix to icon
            static const CIcon &atcSuffixToIcon(const QString &suffix);

            //! Representing icon
            static const CIcon &convertToIcon(const CCallsign &callsign);

            //! Specialized log message for matching / reverse lookup
            //! \threadsafe
            static CStatusMessage logMessage(const CCallsign &callsign, const QString &message,
                                             const QStringList &extraCategories = {},
                                             CStatusMessage::StatusSeverity s = CStatusMessage::SeverityInfo);

            //! Specialized log for matching / reverse lookup
            //! \threadsafe
            static void addLogDetailsToList(CStatusMessageList *log, const CCallsign &callsign, const QString &message,
                                            const QStringList &extraCategories = {},
                                            CStatusMessage::StatusSeverity s = CStatusMessage::SeverityInfo);

            //! Register metadata
            static void registerMetadata();

        private:
            QString m_callsignAsSet;
            QString m_callsign;
            QString m_telephonyDesignator;
            TypeHint m_typeHint = NoHint;

            SWIFT_METACLASS(
                CCallsign,
                SWIFT_METAMEMBER(callsign, 0, CaseInsensitiveComparison),
                SWIFT_METAMEMBER(callsignAsSet, 0, DisabledForComparison | DisabledForHashing),
                SWIFT_METAMEMBER(telephonyDesignator, 0, DisabledForComparison | DisabledForHashing),
                SWIFT_METAMEMBER(typeHint, 0, DisabledForComparison | DisabledForHashing));
        };
    } // namespace aviation
} // namespace swift::misc

Q_DECLARE_METATYPE(swift::misc::aviation::CCallsign)
Q_DECLARE_METATYPE(swift::misc::aviation::CCallsign::TypeHint)

#endif // guard
