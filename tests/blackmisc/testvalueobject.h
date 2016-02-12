/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKMISC_TESTVALUEOBJECT_H
#define BLACKMISC_TESTVALUEOBJECT_H

//! \cond PRIVATE_TESTS

/*!
 * \file
 */

#include "blackmisc/propertyindex.h"
#include "blackmisc/sequence.h"
#include "blackmisc/collection.h"
#include "blackmisc/dictionary.h"
#include "blackmisc/valueobject.h"

namespace BlackMisc
{
    //! Test value object
    class CTestValueObject : public BlackMisc::CValueObject<CTestValueObject>
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

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index);

        //! \copydoc BlackMisc::Mixin::String::toQString()
        QString convertToQString(bool i18n = false) const;

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

//! Test value object dictionary using ordered container
typedef BlackMisc::CDictionary<BlackMisc::CTestValueObject, BlackMisc::CTestValueObject> CValueObjectDictionary;

//! Test value object dictionary using unordered container
typedef BlackMisc::CDictionary<BlackMisc::CTestValueObject, BlackMisc::CTestValueObject, QHash> CValueObjectHashDictionary;

//! Test value object dictionary using ordered container with not hashable key
typedef BlackMisc::CDictionary<BlackMisc::CNotHashable, QString> CNotHashableDictionary;

//! Test value object dictionary using unordered container with not hashable key
typedef BlackMisc::CDictionary<BlackMisc::CNotHashable, QString, QMap> CNotHashableMapDictionary;
Q_DECLARE_METATYPE(CValueObjectDictionary)
Q_DECLARE_METATYPE(CNotHashableDictionary)

// MSVC has trouble with these checks
#if !defined(Q_CC_MSVC)
static_assert(std::is_same<CValueObjectDictionary::impl_type, CValueObjectHashDictionary::impl_type>::value,
              "Expected CValueObjectDictionary to use QHash");
static_assert(std::is_same<CNotHashableDictionary::impl_type, CNotHashableMapDictionary::impl_type>::value,
              "Expected CDictionary<CNotHashableDictionary, Value> to use QMap");
#endif // ! Q_CC_MSVC

//! \endcond

#endif // guard
