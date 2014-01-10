/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#include "valueobject.h"
#include <QVariantMap>
#include <QDBusArgument>

#ifndef BLACKMISC_VALUEMAP_H
#define BLACKMISC_VALUEMAP_H

namespace BlackMisc
{
    /*!
     * \brief Value map
     */
    class CValueMap : public CValueObject
    {

    public:

        /*!
         * \brief Constructor
         * \param wildcard when used in search, for setting values irrelevant
         */
        CValueMap(bool wildcard = false);

        /*!
         * \brief Single value constructor
         * \param index
         * \param value
         */
        CValueMap(int index, const QVariant &value);

        /*!
         * \brief QVariant, required for DBus QVariant lists
         * \return
         */
        virtual QVariant toQVariant() const
        {
            return QVariant::fromValue(*this);
        }

        /*!
         * \brief Destructor
         */
        virtual ~CValueMap() {}

        /*!
         * \brief Add a value
         * \param index
         * \param value
         */
        void addValue(int index, const QVariant &value);

        /*!
         * \brief Add a value as non QVariant
         * \param index
         * \param value
         */
        template<class T> void addValue(int index, const T &value)
        {
            this->m_values.insert(index, QVariant::fromValue(value));
        }

        /*!
         * \brief Is empty
         * \return
         */
        bool isEmpty() const { return this->m_values.isEmpty(); }

        /*!
         * \brief Value
         * \param index
         * \return
         */
        QVariant value(int index) const { return this->m_values.value(index); }

        /*!
         * \brief Indexes
         * \return
         */
        QList<int> indexes() const { return this->m_values.keys(); }

        /*!
         * \brief values
         * \return
         */
        QList<QVariant> values() const { return this->m_values.values(); }

        /*!
         * \brief Wildcard, only relevant when used in search
         * \return
         */
        bool isWildcard() const { return this->m_wildcard; }

        /*!
         * \brief clear
         */
        void clear() { this->m_values.clear(); }

        /*!
         * \brief Map
         * \return
         */
        const QMap<int, QVariant> &map() const { return this->m_values; }

        /*!
         * \brief Value hash
         */
        virtual uint getValueHash() const;

        /*!
         * \brief Metadata
         */
        static void registerMetadata();

    protected:
        QMap<int, QVariant> m_values; /*!< values */
        bool m_wildcard;

        /*!
         * \brief Meaningful string representation
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
    };
}

Q_DECLARE_METATYPE(BlackMisc::CValueMap)

#endif // guard
