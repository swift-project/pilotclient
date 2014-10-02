/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */


#include "valueobject.h"
#include "propertyindexvariantmap.h"
#include "propertyindexlist.h"
#include "propertyindex.h"
#include "iconlist.h"
#include "blackmiscfreefunctions.h"

namespace BlackMisc
{

    /*
     * Stringify
     */
    QString CValueObject::toQString(bool i18n) const
    {
        return this->convertToQString(i18n);
    }

    /*
     * Stringify
     */
    QString CValueObject::toFormattedQString(bool i18n) const
    {
        return this->toQString(i18n);
    }

    /*
     * Stringify
     */
    std::string CValueObject::toStdString(bool i18n) const
    {
        return this->convertToQString(i18n).toStdString();
    }

    /*
     * Streaming
     */
    QString CValueObject::stringForStreaming() const
    {
        // simplest default implementation requires only one method
        return this->convertToQString();
    }

    /*
     * Setter for property by index
     */
    void CValueObject::setPropertyByIndex(const QVariant &variant, const CPropertyIndex &index)
    {
        if (index.isMyself())
        {
            this->convertFromQVariant(variant);
            return;
        }

        // not all classes have implemented nesting
        const QString m = QString("Property by index not found (setter), index: ").append(index.toQString());
        qFatal(m.toLatin1().constData());
    }

    /*
     * By index
     */
    QVariant CValueObject::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
    {
        if (index.isMyself()) { return this->toQVariant(); }
        ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexIcon:
            return this->toIcon().toQVariant();
        case IndexPixmap:
            return QVariant(this->toPixmap());
        case IndexString:
            return QVariant(this->toQString());
        default:
            break;
        }

        // not all classes have implemented nesting
        const QString m = QString("Property by index not found, index: ").append(index.toQString());
        qFatal(m.toLatin1().constData());
        return QVariant(m); // avoid compiler warning
    }

    /*
     * By index as string
     */
    QString CValueObject::propertyByIndexAsString(const CPropertyIndex &index, bool i18n) const
    {
        // default implementation, requires propertyByIndex
        QVariant qv = this->propertyByIndex(index);
        return BlackMisc::qVariantToString(qv, i18n);
    }

    /*
     * Variant equal property index?
     */
    bool CValueObject::equalPropertyByIndex(const QVariant &compareValue, const CPropertyIndex &index) const
    {
        const QVariant myValue = this->propertyByIndex(index);
        return BlackMisc::equalQVariants(myValue, compareValue);
    }

    /*
     * Return backing streamable object (if any)
     */
    const CValueObject *CValueObject::fromQVariant(const QVariant &variant)
    {
        if (!variant.isValid()) return nullptr;
        auto t = static_cast<QMetaType::Type>(variant.type());
        uint ut = variant.userType();
        if (t != QMetaType::User) return nullptr;  // not a user type
        if (ut <= static_cast<uint>(QMetaType::User)) return nullptr; // complex Qt type
        if (variant.canConvert<QDBusArgument>()) return nullptr; // not unstreamed yet

        // this cast cannot be dynamic, so the above conditions are crucical
        const CValueObject *vo = static_cast<const CValueObject *>(variant.constData());
        return vo;
    }

    /*
     * Compare
     */
    int compare(const CValueObject &v1, const CValueObject &v2)
    {
        if (v1.isA(v2.getMetaTypeId()))
        {
            return v2.compareImpl(v1) * -1;
        }
        else if (v2.isA(v1.getMetaTypeId()))
        {
            return v1.compareImpl(v2);
        }
        else
        {
            Q_ASSERT_X(false, Q_FUNC_INFO, "Attempt to compare between instances of unrelated classes");
            return 0;
        }
    }

    /*
     * Variant map
     */
    CPropertyIndexList CValueObject::apply(const BlackMisc::CPropertyIndexVariantMap &indexMap, bool skipEqualValues)
    {
        CPropertyIndexList changed;
        if (indexMap.isEmpty()) return changed;

        const auto &map = indexMap.map();
        for (auto it = map.begin(); it != map.end(); ++it)
        {
            const QVariant value = it.value().toQVariant();
            const CPropertyIndex index = it.key();
            if (skipEqualValues)
            {
                bool equal = this->equalPropertyByIndex(value, index);
                if (equal) { continue; }
            }
            this->setPropertyByIndex(value, index);
            changed.push_back(index);
        }
        return changed;
    }

    /*
     * Icon
     */
    CIcon CValueObject::toIcon() const
    {
        return CIconList::iconForIndex(CIcons::StandardIconUnknown16);
    }

    /*
     * Pixmap
     */
    QPixmap CValueObject::toPixmap() const
    {
        return this->toIcon().toPixmap();
    }

    /*
     * Compare with value map
     */
    bool operator==(const CPropertyIndexVariantMap &indexMap, const CValueObject &valueObject)
    {
        if (indexMap.isEmpty()) return indexMap.isWildcard();
        const auto &map = indexMap.map();
        for (auto it = map.begin(); it != map.end(); ++it)
        {
            // QVariant cannot be compared directly
            QVariant p = valueObject.propertyByIndex(it.key()); // from value object
            QVariant v = it.value().toQVariant(); // from map
            if (!BlackMisc::equalQVariants(p, v)) return false;
        }
        return true;
    }

    /*
     * Compare with map
     */
    bool operator!=(const CPropertyIndexVariantMap &indexMap, const CValueObject &valueObject)
    {
        return !(indexMap == valueObject);
    }

    /*
     * Compare with map
     */
    bool operator==(const CValueObject &valueObject, const CPropertyIndexVariantMap &valueMap)
    {
        return valueMap == valueObject;
    }

    /*
     * Compare with map
     */
    bool operator!=(const CValueObject &valueObject, const CPropertyIndexVariantMap &valueMap)
    {
        return !(valueMap == valueObject);
    }

    /*
     * from DBus
     */
    const QDBusArgument &operator>>(const QDBusArgument &argument, CValueObject &valueObject)
    {
        argument.beginStructure();
        valueObject.unmarshallFromDbus(argument);
        argument.endStructure();
        return argument;
    }

    /*
     * to DBus
     */
    QDBusArgument &operator<<(QDBusArgument &argument, const CValueObject &valueObject)
    {
        argument.beginStructure();
        valueObject.marshallToDbus(argument);
        argument.endStructure();
        return argument;
    }
}
