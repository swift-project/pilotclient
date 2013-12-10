#include "valueobject.h"
#include "valuemap.h"
#include "blackmiscfreefunctions.h"

namespace BlackMisc
{

    /*
     * Constructor
     */
    CValueObject::CValueObject() {}

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
    void CValueObject::setPropertyByIndex(const QVariant & /** variant **/, int /** index **/)
    {
        // not all classes have to implement this
        qFatal("Property by index setter not implemented");
    }

    /*
     * By index
     */
    QVariant CValueObject::propertyByIndex(int /** index **/) const
    {
        // not all classes have to implement this
        qFatal("Property by index not implemented");
        return QVariant("boom"); // avoid compiler warning
    }

    /*
     * By index as string
     */
    QString CValueObject::propertyByIndexAsString(int /** index **/, bool /** i18n **/) const
    {
        // not all classes have to implement this
        qFatal("Property by index as string not implemented");
        return QString("boom"); // avoid compiler warning
    }

    /*
     * Return backing streamable object (if any)
     */
    const CValueObject *CValueObject::fromQVariant(const QVariant &qv)
    {
        if (!qv.isValid()) return nullptr;
        QVariant::Type t = qv.type();
        uint ut = qv.userType();
        if (t != QVariant::UserType) return nullptr;  // not a user type
        if (ut <= QVariant::UserType) return nullptr; // complex Qt type
        if (qv.canConvert<QDBusArgument>()) return nullptr; // not unstreamed yet

        // this cast cannot be dynamic, so the aboce conditions are crucical
        const CValueObject *vo = static_cast<const CValueObject *>(qv.constData());
        return vo;
    }

    /*
     * Compare
     */
    int CValueObject::compare(const QVariant & /** qv **/) const
    {
        // not all classes have to implement this
        qFatal("Property by index as string not implemented");
        return -1; // avoid compiler warning
    }

    /*!
     * Variant map
     */
    int CValueObject::apply(const BlackMisc::CValueMap &valueMap)
    {
        if (valueMap.isEmpty()) return 0;
        int c = 0;

        QMap<int, QVariant>::const_iterator it;
        const QMap<int, QVariant> &map = valueMap.map();
        for (it = map.begin(); it != map.end(); ++it)
        {
            this->setPropertyByIndex(it.value(), it.key());
        }
        return c;
    }

    /*
     * Compare with value map
     */
    bool operator==(const CValueMap &valueMap, const CValueObject &uc)
    {
        if (valueMap.isEmpty()) return valueMap.isWildcard();
        QMap<int, QVariant>::const_iterator it;
        const QMap<int, QVariant> &map = valueMap.map();
        for (it = map.begin(); it != map.end(); ++it)
        {
            // QVariant cannot be compared directly
            QVariant p = uc.propertyByIndex(it.key()); // from value object
            QVariant v = it.value(); // from map
            if (!BlackMisc::equalQVariants(p, v)) return false;
        }
        return true;
    }

    /*
     * Compare with value map
     */
    bool operator!=(const CValueMap &valueMap, const CValueObject &uc)
    {
        return !(valueMap == uc);
    }

    /*
     * Compare with value map
     */
    bool operator==(const CValueObject &uc, const CValueMap &valueMap)
    {
        return valueMap == uc;
    }

    /*
     * Compare with value map
     */
    bool operator!=(const CValueObject &uc, const CValueMap &valueMap)
    {
        return !(valueMap == uc);
    }

    /*
     * from DBus
     */
    const QDBusArgument &operator>>(const QDBusArgument &argument, CValueObject &uc)
    {
        argument.beginStructure();
        uc.unmarshallFromDbus(argument);
        argument.endStructure();
        return argument;
    }

    /*
     * to DBus
     */
    QDBusArgument &operator<<(QDBusArgument &argument, const CValueObject &uc)
    {
        argument.beginStructure();
        uc.marshallToDbus(argument);
        argument.endStructure();
        return argument;
    }
}

uint qHash(const BlackMisc::CValueObject &value)
{
    return value.getValueHash();
}
