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
        class CCallsign : public BlackMisc::CValueObject
        {

        public:
            //! Indexes
            enum ColumnIndex
            {
                IndexCallsignString = BlackMisc::CPropertyIndex::GlobalIndexCCallsign,
                IndexCallsignStringAsSet,
                IndexTelephonyDesignator
            };

            //! Default constructor.
            CCallsign() {}

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

            //! Equals callsign string?
            bool equalsString(const QString &callsignString) const;

            //! Equal operator ==
            bool operator ==(const CCallsign &other) const;

            //! Unequal operator !=
            bool operator !=(const CCallsign &other) const;

            //! Less than operator < for sorting
            bool operator <(const CCallsign &other) const;

            //! \copydoc CValueObject::getValueHash()
            virtual uint getValueHash() const override;

            //! \copydoc CValueObject::toJson
            virtual QJsonObject toJson() const override;

            //! \copydoc CValueObject::convertFromJson
            virtual void convertFromJson(const QJsonObject &json) override;

            //! \copydoc CValueObject::toQVariant()
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

            //! \copydoc CValueObject::convertFromQVariant
            virtual void convertFromQVariant(const QVariant &variant) override { BlackMisc::setFromQVariant(this, variant); }

            //! \copydoc CValueObject::toIcon()
            virtual BlackMisc::CIcon toIcon() const override { return CCallsign::convertToIcon(*this); }

            //! \copydoc CValueObject::propertyByIndex
            virtual QVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const override;

            //! \copydoc CValueObject::setPropertyByIndex(variant, index)
            virtual void setPropertyByIndex(const QVariant &variant, const BlackMisc::CPropertyIndex &index) override;

            //! Register metadata
            static void registerMetadata();

            //! Members
            static const QStringList &jsonMembers();

        protected:
            //! \copydoc CValueObject::convertToQString()
            virtual QString convertToQString(bool i18n = false) const override;

            //! \copydoc CValueObject::getMetaTypeId
            virtual int getMetaTypeId() const override;

            //! \copydoc CValueObject::isA
            virtual bool isA(int metaTypeId) const override;

            //! \copydoc CValueObject::compareImpl
            virtual int compareImpl(const CValueObject &other) const override;

            //! \copydoc CValueObject::marshallToDbus()
            virtual void marshallToDbus(QDBusArgument &argument) const override;

            //! \copydoc CValueObject::unmarshallFromDbus()
            virtual void unmarshallFromDbus(const QDBusArgument &argument) override;

            /*!
             * Unify the callsign
             * \param callsign
             * \return unified callsign
             */
            static QString unifyCallsign(const QString &callsign);

            //! representing icon
            static const CIcon &convertToIcon(const CCallsign &callsign);

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CCallsign)
            QString m_callsignAsSet;
            QString m_callsign;
            QString m_telephonyDesignator;
        };
    } // namespace
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Aviation::CCallsign, (o.m_callsign, o.m_callsignAsSet, o.m_telephonyDesignator))
Q_DECLARE_METATYPE(BlackMisc::Aviation::CCallsign)

#endif // guard
