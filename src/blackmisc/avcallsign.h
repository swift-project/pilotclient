/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#ifndef BLACKMISC_CALLSIGN_H
#define BLACKMISC_CALLSIGN_H
#include "valueobject.h"

namespace BlackMisc
{
    namespace Aviation
    {
        //! \brief Value object encapsulating information of a callsign.
        class CCallsign : public BlackMisc::CValueObject
        {

        public:
            //! \brief Default constructor.
            CCallsign() {}

            //! \brief Constructor
            CCallsign(const QString &callsign, const QString &telephonyDesignator = "")
                : m_callsignAsSet(callsign.trimmed()), m_callsign(CCallsign::unifyCallsign(callsign.trimmed())), m_telephonyDesignator(telephonyDesignator.trimmed())
            {}

            //! \brief Constructor, needed to disambiguate implicit conversion from string literal.
            CCallsign(const char *callsign)
                : m_callsignAsSet(callsign), m_callsign(CCallsign::unifyCallsign(callsign))
            {}

            //! \copydoc CValueObject::toQVariant()
            virtual QVariant toQVariant() const override
            {
                return QVariant::fromValue(*this);
            }

            //! \copydoc CValueObject::toIcon()
            virtual const QPixmap &toIcon() const override
            {
                return CCallsign::convertToIcon(*this);
            }

            //! \brief Is empty?
            bool isEmpty() const { return this->m_callsignAsSet.isEmpty(); }

            //! \brief Get callsign.
            const QString &asString() const { return this->m_callsign; }

            //! \brief Get callsign.
            const QString &getStringAsSet() const { return this->m_callsignAsSet; }

            //! \brief Get callsign telephony designator (how callsign is pronounced)
            const QString &getTelephonyDesignator() const { return this->m_telephonyDesignator; }

            //! \brief Get ICAO code, if this makes sense (EDDF_TWR -> EDDF)
            QString getIcaoCode() const { return m_callsign.left(4).toUpper(); }

            //! \brief Makes this callsign looking like an observer callsign (DAMBZ -> DAMBZ_OBS)
            QString getAsObserverCallsignString() const;

            //! \brief Equals callsign string?
            bool equalsString(const QString &callsignString) const;

            //! \brief Equal operator ==
            bool operator ==(const CCallsign &other) const;

            //! \brief Unequal operator !=
            bool operator !=(const CCallsign &other) const;

            //! \brief Less than operator < for sorting
            bool operator <(const CCallsign &other) const;

            //! \copydoc CValueObject::getValueHash()
            virtual uint getValueHash() const override;

            //! \brief Register metadata
            static void registerMetadata();

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
             * \brief Unify the callsign
             * \param callsign
             * \return unified callsign
             */
            static QString unifyCallsign(const QString &callsign);

            //! \brief representing icon
            static const QPixmap &convertToIcon(const CCallsign &callsign);

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
