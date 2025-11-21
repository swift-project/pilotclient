// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef SWIFT_MISC_TESTVALUEOBJECT_H
#define SWIFT_MISC_TESTVALUEOBJECT_H

//! \cond PRIVATE_TESTS
//! \file
//! \ingroup testmisc

#include <type_traits>

#include <QDBusArgument>
#include <QHash>
#include <QMap>
#include <QMetaType>
#include <QString>

#include "misc/collection.h"
#include "misc/dictionary.h"
#include "misc/metaclass.h"
#include "misc/propertyindex.h"
#include "misc/sequence.h"
#include "misc/valueobject.h"
#include "misc/variant.h"

class QJsonArray;
class QJsonValueRef;

namespace swift::misc
{
    //! Test value object
    class CTestValueObject : public swift::misc::CValueObject<CTestValueObject>
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexName = 10000,
            IndexDescription,
        };

        //! Default constructor.
        CTestValueObject() = default;

        //! Constructor.
        CTestValueObject(const QString &name, const QString &description) : m_name(name), m_description(description) {}

        //! Get name
        const QString &getName() const { return m_name; }

        //! Set name
        void setName(const QString &name) { m_name = name; }

        //! Get description
        const QString &getDescription() const { return m_description; }

        //! Set description
        void setDescription(const QString &description) { m_description = description; }

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        CVariant propertyByIndex(const swift::misc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            const auto i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexDescription: return CVariant::fromValue(this->m_description);
            case IndexName: return CVariant::fromValue(this->m_name);
            default: return CValueObject::propertyByIndex(index);
            }
        }

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(const swift::misc::CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself())
            {
                (*this) = variant.to<CTestValueObject>();
                return;
            }
            const auto i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexDescription: this->setDescription(variant.value<QString>()); break;
            case IndexName: this->setName(variant.value<QString>()); break;
            default: CValueObject::setPropertyByIndex(index, variant); break;
            }
        }

        //! \copydoc swift::misc::mixin::String::toQString()
        QString convertToQString(bool i18n = false) const
        {
            Q_UNUSED(i18n);
            QString s(this->m_name);
            s.append(" ").append(this->m_description);
            return s;
        }

    private:
        QString m_name;
        QString m_description;

        SWIFT_METACLASS(
            CTestValueObject,
            SWIFT_METAMEMBER(name),
            SWIFT_METAMEMBER(description));
    };

    //! \cond NO_DOXYGEN
    struct CNotHashable
    {
        int n;
        bool operator<(const CNotHashable &other) const { return n < other.n; }
        QString toQString(bool = false) const { return {}; }
        bool operator==(const CNotHashable &other) const { return n == other.n; }
    };
    inline QJsonArray &operator<<(QJsonArray &a, const CNotHashable &) { return a; }
    inline const QJsonValueRef &operator>>(const QJsonValueRef &v, CNotHashable &) { return v; }
    inline QDBusArgument &operator<<(QDBusArgument &a, const CNotHashable &) { return a; }
    inline const QDBusArgument &operator>>(const QDBusArgument &a, const CNotHashable &) { return a; }
    inline QDataStream &operator<<(QDataStream &a, const CNotHashable &) { return a; }
    inline QDataStream &operator>>(QDataStream &a, const CNotHashable &) { return a; }
    //! \endcond

} // namespace swift::misc

Q_DECLARE_METATYPE(swift::misc::CTestValueObject)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::misc::CTestValueObject>)
Q_DECLARE_METATYPE(swift::misc::CSequence<swift::misc::CTestValueObject>)

Q_DECLARE_METATYPE(swift::misc::CNotHashable)

// We need to typedef because 'commas' confuse the Q_DECLARE_METATYPE macro
// https://bugreports.qt-project.org/browse/QTBUG-11485

//! Test value object dictionary using ordered container
using CValueObjectDictionary = swift::misc::CDictionary<swift::misc::CTestValueObject, swift::misc::CTestValueObject>;

//! Test value object dictionary using unordered container
using CValueObjectHashDictionary =
    swift::misc::CDictionary<swift::misc::CTestValueObject, swift::misc::CTestValueObject, QHash>;

//! Test value object dictionary using ordered container with not hashable key
using CNotHashableDictionary = swift::misc::CDictionary<swift::misc::CNotHashable, QString>;

//! Test value object dictionary using unordered container with not hashable key
using CNotHashableMapDictionary = swift::misc::CDictionary<swift::misc::CNotHashable, QString, QMap>;
Q_DECLARE_METATYPE(CValueObjectDictionary)
Q_DECLARE_METATYPE(CNotHashableDictionary)

static_assert(std::is_same<CValueObjectDictionary::impl_type, CValueObjectHashDictionary::impl_type>::value,
              "Expected CValueObjectDictionary to use QHash");
static_assert(std::is_same<CNotHashableDictionary::impl_type, CNotHashableMapDictionary::impl_type>::value,
              "Expected CDictionary<CNotHashableDictionary, Value> to use QMap");

//! \endcond

#endif // SWIFT_MISC_TESTVALUEOBJECT_H
