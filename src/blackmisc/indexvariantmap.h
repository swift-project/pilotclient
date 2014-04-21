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

#ifndef BLACKMISC_INDEXVARIANTMAP_H
#define BLACKMISC_INDEXVARIANTMAP_H

namespace BlackMisc
{
    /*!
     * Specialized value object compliant map for variants,
     * based on Column indexes
     */
    class CIndexVariantMap : public CValueObject
    {

    public:

        /*!
         * \brief Constructor
         * \param wildcard when used in search, for setting values irrelevant
         */
        CIndexVariantMap(bool wildcard = false);

        //! Single value constructor
        CIndexVariantMap(int index, const QVariant &value);

        //! \copydoc CValueObject::toQVariant
        virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

        //! Destructor
        virtual ~CIndexVariantMap() {}

        //! Add a value
        void addValue(int index, const QVariant &value);

        //! Add a value as non QVariant
        template<class T> void addValue(int index, const T &value) { this->m_values.insert(index, QVariant::fromValue(value)); }

        //! Is empty?
        bool isEmpty() const { return this->m_values.isEmpty(); }

        //! Value
        QVariant value(int index) const { return this->m_values.value(index); }

        //! Indexes
        QList<int> indexes() const { return this->m_values.keys(); }

        //! values
        QList<QVariant> values() const { return this->m_values.values(); }

        //! \brief Wildcard, only relevant when used in search
        bool isWildcard() const { return this->m_wildcard; }

        //! \brief Wildcard, only relevant when used in search
        void setWildcard(bool wildcard) { this->m_wildcard = wildcard; }

        //! clear
        void clear() { this->m_values.clear(); }

        //! Equal operator, required if maps are directly compared, not with CValueObject
        bool operator ==(const CIndexVariantMap &other) const;

        //! Equal operator, required if maps are directly compared, not with CValueObject
        bool operator !=(const CIndexVariantMap &other) const;

        //! Map
        const QMap<int, QVariant> &map() const { return this->m_values; }

        //! \copydoc CValueObject::getValueHash
        virtual uint getValueHash() const override;

        //! register metadata
        static void registerMetadata();

    protected:
        QMap<int, QVariant> m_values; //!< values
        bool m_wildcard; //!< wildcard

        //! \copydoc CValueObject::convertToQString
        virtual QString convertToQString(bool i18n = false) const override;

        //! \copydoc CValueObject::getMetaTypeId
        virtual int getMetaTypeId() const override;

        //! \copydoc CValueObject::isA
        virtual bool isA(int metaTypeId) const override;

        //! \copydoc CValueObject::compareImpl
        virtual int compareImpl(const CValueObject &other) const override;

        //! \copydoc CValueObject::marshallToDbus
        virtual void marshallToDbus(QDBusArgument &argument) const override;

        //! \copydoc CValueObject::unmarshallFromDbus
        virtual void unmarshallFromDbus(const QDBusArgument &argument) override;
    };
}

Q_DECLARE_METATYPE(BlackMisc::CIndexVariantMap)

#endif // guard
