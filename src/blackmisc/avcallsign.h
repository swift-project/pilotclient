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
        /*!
         * Value object encapsulating information of a callsign.
         */
        class CCallsign : public BlackMisc::CValueObject
        {

        public:
            /*!
             * Default constructor.
             */
            CCallsign() {}

            /*!
             * Constructor.
             */
            CCallsign(const QString &callsign)
                : m_callsignAsSet(callsign), m_callsign(CCallsign::unifyCallsign(callsign))
            {}

            /*!
             * Constructor.
             * Needed to disambiguate implicit conversion from string literal.
             */
            CCallsign(const char *callsign)
                : m_callsignAsSet(callsign), m_callsign(CCallsign::unifyCallsign(callsign))
            {}

            /*!
             * \brief Virtual method to return QVariant, used with DBUS QVariant lists
             * \return
             */
            virtual QVariant toQVariant() const
            {
                return QVariant::fromValue(*this);
            }

            /*!
             * \brief Is empty?
             * \return
             */
            bool isEmpty() const { return this->m_callsignAsSet.isEmpty(); }

            /*!
             * Get callsign.
             * \return
             */
            const QString &asString() const { return this->m_callsign; }

            /*!
             * Get callsign.
             * \return
             */
            const QString &getStringAsSet() const { return this->m_callsignAsSet; }

            /*!
             * \brief Get ICAO code, if this possible
             * \return
             */
            QString getIcaoCode() const { return m_callsign.left(4).toUpper(); }

            /*!
             * \brief Equals callsign string?
             * \param callsignString
             * \return
             */
            bool equalsString(const QString &callsignString) const;

            /*!
             * \brief Equal operator ==
             * \param other
             * \return
             */
            bool operator ==(const CCallsign &other) const;

            /*!
             * \brief Unequal operator ==
             * \param other
             * \return
             */
            bool operator !=(const CCallsign &other) const;

            /*!
             * \brief Less than operator < for sorting
             * \param other
             * \return
             */
            bool operator <(const CCallsign &other) const;

            /*!
             * \brief Value hash
             * \return
             */
            virtual uint getValueHash() const;

            /*!
             * \copydoc BlackObject::compare
             */
            virtual int compare(const QVariant &qv) const;

            /*!
             * Compare with other callsign
             * \return
             */
            virtual int compare(const CCallsign &callsign) const;

            /*!
             * \brief Register metadata
             */
            static void registerMetadata();

        protected:
            /*!
             * \brief Rounded value as string
             * \param i18n
             * \return
             */
            virtual QString convertToQString(bool i18n = false) const;

            /*!
             * \brief Stream to DBus <<
             * \param argument
             */
            virtual void marshallToDbus(QDBusArgument &argument) const;

            /*!
             * \brief Stream from DBus >>
             * \param argument
             */
            virtual void unmarshallFromDbus(const QDBusArgument &argument);

            /*!
             * \brief Unify the callsign
             * \param callsign
             * \return
             */
            static QString unifyCallsign(const QString &callsign);

        private:
            QString m_callsignAsSet;
            QString m_callsign;
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CCallsign)

#endif // guard
