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
         * \copydoc CValueObject::toQVariant
         */
        virtual QVariant toQVariant() const override
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
         * \copydoc CValueObject::getValueHash
         */
        virtual uint getValueHash() const override;

        /*!
         * \brief Metadata
         */
        static void registerMetadata();

    protected:
        QMap<int, QVariant> m_values; //!< values
        bool m_wildcard; //!< wildcard

        /*!
         * \copydoc CValueObject::convertToQString
         */
        virtual QString convertToQString(bool i18n = false) const override;

        /*!
         * \copydoc CValueObject::getMetaTypeId
         */
        virtual int getMetaTypeId() const override;

        /*!
         * \copydoc CValueObject::isA
         */
        virtual bool isA(int metaTypeId) const override;

        /*!
         * \copydoc CValueObject::compareImpl
         */
        virtual int compareImpl(const CValueObject &other) const override;

        /*!
         * \copydoc CValueObject::marshallToDbus
         */
        virtual void marshallToDbus(QDBusArgument &argument) const override;

        /*!
         * \copydoc CValueObject::unmarshallFromDbus
         */
        virtual void unmarshallFromDbus(const QDBusArgument &argument) override;
    };
}

Q_DECLARE_METATYPE(BlackMisc::CValueMap)

#endif // guard
