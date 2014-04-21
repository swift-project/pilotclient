#include "indexvariantmap.h"
#include "blackmiscfreefunctions.h"
#include "avaltitude.h"

namespace BlackMisc
{
    /*
     * Constructor
     */
    CIndexVariantMap::CIndexVariantMap(bool wildcard) : m_wildcard(wildcard) {}

    /*
     * Constructor single value
     */
    CIndexVariantMap::CIndexVariantMap(int index, const QVariant &value)
    {
        this->addValue(index, value);
    }

    /*
     * ==
     */
    bool CIndexVariantMap::operator ==(const CIndexVariantMap &other) const
    {
        return this->m_wildcard == other.m_wildcard && this->m_values == other.m_values;
    }

    /*
     * !=
     */
    bool CIndexVariantMap::operator !=(const CIndexVariantMap &other) const
    {
        return !(this->operator ==(other));
    }

    /*
     * Convert to string
     */
    QString CIndexVariantMap::convertToQString(bool i18n) const
    {
        if (this->isEmpty()) return QString("{wildcard: %1}").arg(this->m_wildcard ? "true" : "false");
        QString s;
        foreach(int index, this->m_values.keys())
        {
            QVariant qv = this->m_values.value(index);

            s.isEmpty() ?
            s.append("{wildcard: ").append(this->m_wildcard ? "true" : "false").append(" ") :
            s.append(", ");

            s.append('{').append(QString::number(index)).append(": ");
            s.append("(").append(QString::number(qv.userType())).append(") ");
            QString qvs = BlackMisc::qVariantToString(qv, i18n);
            s.append(qvs);
            s.append('}');
        }
        s = s.append("}");
        return s;
    }

    /*
     * metaTypeId
     */
    int CIndexVariantMap::getMetaTypeId() const
    {
        return qMetaTypeId<CIndexVariantMap>();
    }

    /*
     * is a
     */
    bool CIndexVariantMap::isA(int metaTypeId) const
    {
        if (metaTypeId == qMetaTypeId<CIndexVariantMap>()) { return true; }

        return this->CValueObject::isA(metaTypeId);
    }

    /*
     * Compare
     */
    int CIndexVariantMap::compareImpl(const CValueObject &/*otherBase*/) const
    {
        qFatal("not implemented");
        return 0;
    }

    /*
     * Marshall to DBus
     */
    void CIndexVariantMap::marshallToDbus(QDBusArgument &argument) const
    {
        // remark, tried both sending as QDbusVariant and QVariant
        // does not make a difference
        QList<int> unifiedBlackTypeIds;
        QList<QDBusVariant> dbusVariants;
        foreach(QVariant qv, m_values.values())
        {
            unifiedBlackTypeIds << qv.userType() - BlackMisc::firstBlackMetaType();
            dbusVariants << QDBusVariant(qv);
        }
        argument << this->m_values.keys(); // indexes
        argument << dbusVariants;
        argument << unifiedBlackTypeIds;
    }

    /*
     * Unmarshall from DBus
     */
    void CIndexVariantMap::unmarshallFromDbus(const QDBusArgument &argument)
    {
        QList<int> indexes;
        QList<QDBusVariant> values;
        QList<int> unifiedBlackTypeIds;
        argument >> indexes;
        argument >> values;
        argument >> unifiedBlackTypeIds;
        QMap<int, QVariant> newMap;
        for (int i = 0; i < indexes.size(); i++)
        {
            QVariant qv = values.at(i).variant();
            int index = indexes.at(i);
            if (qv.canConvert<QDBusArgument>())
            {
                int userType = unifiedBlackTypeIds.at(i) + BlackMisc::firstBlackMetaType();
                QVariant concrete = BlackMisc::fixQVariantFromDbusArgument(qv, userType);
                newMap.insert(index, concrete);
            }
            else
            {
                // value already OK
                newMap.insert(index, qv);
            }
        }
        // replace values in one step
        this->m_values.clear();
        this->m_values.unite(newMap);
    }

    /*
     * Add value
     */
    void CIndexVariantMap::addValue(int index, const QVariant &value)
    {
        this->m_values.insert(index, value);
    }

    /*
     * Register metadata
     */
    void CIndexVariantMap::registerMetadata()
    {
        qRegisterMetaType<CIndexVariantMap>();
        qDBusRegisterMetaType<CIndexVariantMap>();
    }

    /*
     * Hash
     */
    uint CIndexVariantMap::getValueHash() const
    {
        // there is no hash for map, so I use this workaround here
        const QString s = this->toQString(false);
        QList<uint> h;
        h << qHash(s);
        return BlackMisc::calculateHash(h, "CIndexVariantMap");
    }
} // namespace
