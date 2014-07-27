/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_NAMEVARIANTPAIR_H
#define BLACKMISC_NAMEVARIANTPAIR_H

#include "variant.h"
#include "valueobject.h"

namespace BlackMisc
{
    /*!
     * Value / variant pair
     */
    class CNameVariantPair : public BlackMisc::CValueObject
    {
    public:
        //! Default constructor.
        CNameVariantPair() {}

        //! Constructor.
        CNameVariantPair(const QString &name, const CVariant &variant);

        //! \copydoc CValueObject::toQVariant
        virtual QVariant toQVariant() const override
        {
            return QVariant::fromValue(*this);
        }

        //! Get name.
        const QString &getName() const { return m_name; }

        //! Get variant.
        CVariant getVariant() const { return m_variant; }

        //! Set name.
        void setName(const QString &name) { this->m_name = name; }

        //! Set variant.
        void setVariant(const CVariant &variant) { m_variant = variant; }

        //! Equal operator ==
        bool operator ==(const CNameVariantPair &other) const;

        //! Unequal operator !=
        bool operator !=(const CNameVariantPair &other) const;

        //! \copydoc CValueObject::getValueHash
        virtual uint getValueHash() const override;

        //! \copydoc CValueObject::toJson
        virtual QJsonObject toJson() const override;

        //! \copydoc CValueObject::fromJson
        virtual void fromJson(const QJsonObject &json) override;

        //! Register metadata
        static void registerMetadata();

        //! Members
        static const QStringList &jsonMembers();

        //! Properties by index
        enum ColumnIndex
        {
            IndexName = 0,
            IndexVariant,
        };

        //! \copydoc CValueObject::propertyByIndex()
        virtual QVariant propertyByIndex(int index) const override;

        //! \copydoc CValueObject::setPropertyByIndex(variant, index)
        virtual void setPropertyByIndex(const QVariant &variant, int index) override;

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

        //! \copydoc CValueObject::marshallFromDbus()
        virtual void unmarshallFromDbus(const QDBusArgument &argument) override;

    private:
        BLACK_ENABLE_TUPLE_CONVERSION(CNameVariantPair)
        QString m_name;
        CVariant m_variant;
    };
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::CNameVariantPair, (o.m_name, o.m_variant))
Q_DECLARE_METATYPE(BlackMisc::CNameVariantPair)

#endif // guard
