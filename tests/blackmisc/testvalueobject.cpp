#include "testvalueobject.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include <tuple>

namespace BlackMisc
{
    /*
     * Convert to string
     */
    QString CTestValueObject::convertToQString(bool /*i18n*/) const
    {
        QString s(this->m_name);
        s.append(" ").append(this->m_description);
        return s;
    }

    /*
     * metaTypeId
     */
    int CTestValueObject::getMetaTypeId() const
    {
        return qMetaTypeId<CTestValueObject>();
    }

    /*
     * is a
     */
    bool CTestValueObject::isA(int metaTypeId) const
    {
        if (metaTypeId == qMetaTypeId<CTestValueObject>()) { return true; }

        return this->CValueObject::isA(metaTypeId);
    }

    /*
     * Compare
     */
    int CTestValueObject::compareImpl(const CValueObject &otherBase) const
    {
        const auto &other = static_cast<const CTestValueObject &>(otherBase);

        return compare(TupleConverter<CTestValueObject>::toTuple(*this), TupleConverter<CTestValueObject>::toTuple(other));
    }

    /*
     * Marshall to DBus
     */
    void CTestValueObject::marshallToDbus(QDBusArgument &argument) const
    {
        argument << TupleConverter<CTestValueObject>::toTuple(*this);
    }

    /*
     * Unmarshall from DBus
     */
    void CTestValueObject::unmarshallFromDbus(const QDBusArgument &argument)
    {
        argument >> TupleConverter<CTestValueObject>::toTuple(*this);
    }

    /*
     * Equal?
     */
    bool CTestValueObject::operator ==(const CTestValueObject &other) const
    {
        if (this == &other) return true;
        return TupleConverter<CTestValueObject>::toTuple(*this) == TupleConverter<CTestValueObject>::toTuple(other);
    }

    /*
     * Unequal?
     */
    bool CTestValueObject::operator !=(const CTestValueObject &other) const
    {
        return !((*this) == other);
    }

    /*
     * Hash
     */
    uint CTestValueObject::getValueHash() const
    {
        return qHash(TupleConverter<CTestValueObject>::toTuple(*this));
    }

    /*
     * Property by index
     */
    QVariant CTestValueObject::propertyByIndex(int index) const
    {
        switch (index)
        {
        case IndexDescription:
            return QVariant::fromValue(this->m_description);
        case IndexName:
            return QVariant::fromValue(this->m_name);
        default:
            break;
        }

        Q_ASSERT_X(false, "CTestValueObject", "index unknown");
        QString m = QString("no property, index ").append(QString::number(index));
        return QVariant::fromValue(m);
    }

    /*
     * Property as string by index
     */
    QString CTestValueObject::propertyByIndexAsString(int index, bool i18n) const
    {
        QVariant qv = this->propertyByIndex(index);
        return BlackMisc::qVariantToString(qv, i18n);
    }

    /*
     * Property by index (setter)
     */
    void CTestValueObject::setPropertyByIndex(const QVariant &variant, int index)
    {
        switch (index)
        {
        case IndexDescription:
            this->setDescription(variant.value<QString>());
            break;
        case IndexName:
            this->setName(variant.value<QString>());
            break;
        default:
            Q_ASSERT_X(false, "CTestValueObject", "index unknown");
            break;
        }
    }

    /*
     * Register metadata
     */
    void CTestValueObject::registerMetadata()
    {
        qRegisterMetaType<CTestValueObject>();
        qDBusRegisterMetaType<CTestValueObject>();
    }

    /*
     * Members
     */
    const QStringList &CTestValueObject::jsonMembers()
    {
        return TupleConverter<CTestValueObject>::jsonMembers();
    }

    /*
     * To JSON
     */
    QJsonObject CTestValueObject::toJson() const
    {
        return BlackMisc::serializeJson(CTestValueObject::jsonMembers(), TupleConverter<CTestValueObject>::toTuple(*this));
    }

    /*
     * From Json
     */
    void CTestValueObject::fromJson(const QJsonObject &json)
    {
        BlackMisc::deserializeJson(json, CTestValueObject::jsonMembers(), TupleConverter<CTestValueObject>::toTuple(*this));
    }

} // namespace
