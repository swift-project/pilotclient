// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef BLACKMISC_TESTVALUEOBJECT_H
#define BLACKMISC_TESTVALUEOBJECT_H

//! \cond PRIVATE_TESTS
//! \file
//! \ingroup testblackmisc

#include "blackmisc/collection.h"
#include "blackmisc/dictionary.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/sequence.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/variant.h"

#include <QDBusArgument>
#include <QHash>
#include <QMap>
#include <QMetaType>
#include <QString>
#include <type_traits>

class QJsonArray;
class QJsonValueRef;

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
        CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexDescription:
                return CVariant::fromValue(this->m_description);
            case IndexName:
                return CVariant::fromValue(this->m_name);
            default:
                return CValueObject::propertyByIndex(index);
            }
        }

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(const BlackMisc::CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself())
            {
                (*this) = variant.to<CTestValueObject>();
                return;
            }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexDescription:
                this->setDescription(variant.value<QString>());
                break;
            case IndexName:
                this->setName(variant.value<QString>());
                break;
            default:
                CValueObject::setPropertyByIndex(index, variant);
                break;
            }
        }

        //! \copydoc BlackMisc::Mixin::String::toQString()
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

        BLACK_METACLASS(
            CTestValueObject,
            BLACK_METAMEMBER(name),
            BLACK_METAMEMBER(description)
        );
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

} // namespace

Q_DECLARE_METATYPE(BlackMisc::CTestValueObject)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::CTestValueObject>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::CTestValueObject>)

Q_DECLARE_METATYPE(BlackMisc::CNotHashable)

// We need to typedef because 'commas' confuse the Q_DECLARE_METATYPE macro
// https://bugreports.qt-project.org/browse/QTBUG-11485

//! Test value object dictionary using ordered container
using CValueObjectDictionary = BlackMisc::CDictionary<BlackMisc::CTestValueObject, BlackMisc::CTestValueObject>;

//! Test value object dictionary using unordered container
using CValueObjectHashDictionary = BlackMisc::CDictionary<BlackMisc::CTestValueObject, BlackMisc::CTestValueObject, QHash>;

//! Test value object dictionary using ordered container with not hashable key
using CNotHashableDictionary = BlackMisc::CDictionary<BlackMisc::CNotHashable, QString>;

//! Test value object dictionary using unordered container with not hashable key
using CNotHashableMapDictionary = BlackMisc::CDictionary<BlackMisc::CNotHashable, QString, QMap>;
Q_DECLARE_METATYPE(CValueObjectDictionary)
Q_DECLARE_METATYPE(CNotHashableDictionary)

static_assert(std::is_same<CValueObjectDictionary::impl_type, CValueObjectHashDictionary::impl_type>::value,
              "Expected CValueObjectDictionary to use QHash");
static_assert(std::is_same<CNotHashableDictionary::impl_type, CNotHashableMapDictionary::impl_type>::value,
              "Expected CDictionary<CNotHashableDictionary, Value> to use QMap");

//! \endcond

#endif // guard
