/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SERVER_H
#define BLACKMISC_SERVER_H

#include "blackmisc/propertyindex.h"
#include "blackmisc/sequence.h"
#include "blackmisc/collection.h"
#include "blackmisc/dictionary.h"

namespace BlackMisc
{
    /*!
     * Value object encapsulating information of a server
     */
    class CTestValueObject : public BlackMisc::CValueObject
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexName = 10000,
            IndexDescription,
        };

        //! Default constructor.
        CTestValueObject() {}

        //! Constructor.
        CTestValueObject(const QString &name, const QString &description)
            : m_name(name), m_description(description) {}


        //! Get name
        const QString &getName() const { return m_name; }

        //! Set name
        void setName(const QString &name) { m_name = name; }

        //! Get description
        const QString &getDescription() const { return m_description; }

        //! Set description
        void setDescription(const QString &description) { m_description = description; }

        //! Equal operator ==
        bool operator ==(const CTestValueObject &other) const;

        //! Unequal operator !=
        bool operator !=(const CTestValueObject &other) const;

        //! \copydoc CValueObject::getValueHash()
        virtual uint getValueHash() const override;

        //! \copydoc CValueObject::toJson
        virtual QJsonObject toJson() const override;

        //! \copydoc CValueObject::fromJson
        void fromJson(const QJsonObject &json) override;

        //! Register metadata
        static void registerMetadata();

        //! \copydoc TupleConverter<>::jsonMembers()
        static const QStringList &jsonMembers();

        //! \copydoc CValueObject::propertyByIndex(int)
        virtual QVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const override;

        //! \copydoc CValueObject::setPropertyByIndex(const QVariant &, int index)
        void setPropertyByIndex(const QVariant &variant, const BlackMisc::CPropertyIndex &index) override;

        //! \copydoc CValueObject::toQVariant
        virtual QVariant toQVariant() const override
        {
            return QVariant::fromValue(*this);
        }

        //! \copydoc CValueObject::fromQVariant
        virtual void fromQVariant(const QVariant &variant) override
        {
            Q_ASSERT(variant.canConvert<CTestValueObject>());
            if (variant.canConvert<CTestValueObject>())
            {
                (*this) = variant.value<CTestValueObject>();
            }
        }

    protected:
        //! \copydoc CValueObject::convertToQString()
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

    private:
        BLACK_ENABLE_TUPLE_CONVERSION(CTestValueObject)
        QString m_name;
        QString m_description;
    };

    //! \cond NO_DOXYGEN
    struct CNotHashable
    {
        int n;
        bool operator <(const CNotHashable &other) const { return n < other.n; }
        QString toQString(bool = false) const { return {}; }
    };
    inline QJsonArray &operator <<(QJsonArray &a, const CNotHashable &) { return a; }
    inline const QJsonValueRef &operator >>(const QJsonValueRef &v, CNotHashable &) { return v; }
    inline QDBusArgument &operator <<(QDBusArgument &a, const CNotHashable &) { return a; }
    inline const QDBusArgument &operator >>(const QDBusArgument &a, const CNotHashable &) { return a; }
    //! \endcond

} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::CTestValueObject, (o.m_name, o.m_description))
Q_DECLARE_METATYPE(BlackMisc::CTestValueObject)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::CTestValueObject>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::CTestValueObject>)

Q_DECLARE_METATYPE(BlackMisc::CNotHashable)

// We need to typedef because 'commas' confuse the Q_DECLARE_METATYPE macro
// https://bugreports.qt-project.org/browse/QTBUG-11485
typedef BlackMisc::CDictionary<BlackMisc::CTestValueObject, BlackMisc::CTestValueObject> CValueObjectDictionary;
typedef BlackMisc::CDictionary<BlackMisc::CTestValueObject, BlackMisc::CTestValueObject, QHash> CValueObjectHashDictionary;
typedef BlackMisc::CDictionary<BlackMisc::CNotHashable, QString> CNotHashableDictionary;
typedef BlackMisc::CDictionary<BlackMisc::CNotHashable, QString, QMap> CNotHashableMapDictionary;
Q_DECLARE_METATYPE(CValueObjectDictionary)
Q_DECLARE_METATYPE(CValueObjectHashDictionary)
Q_DECLARE_METATYPE(CNotHashableDictionary)
Q_DECLARE_METATYPE(CNotHashableMapDictionary)

// MSVC has trouble with these checks
#if !defined(Q_CC_MSVC)
static_assert(std::is_same<CValueObjectDictionary::impl_type, CValueObjectHashDictionary::impl_type>::value,
              "Expected CValueObjectDictionary to use QHash");
static_assert(std::is_same<CNotHashableDictionary::impl_type, CNotHashableMapDictionary::impl_type>::value,
              "Expected CDictionary<CNotHashableDictionary, Value> to use QMap");
#endif // ! Q_CC_MSVC

#endif // guard
