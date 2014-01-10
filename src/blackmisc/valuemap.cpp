#include "valuemap.h"
#include "blackmiscfreefunctions.h"
#include "avaltitude.h"

namespace BlackMisc
{
    /*
     * Constructor
     */
    CValueMap::CValueMap(bool wildcard) : m_wildcard(wildcard) {}

    /*
     * Constructor single value
     */
    CValueMap::CValueMap(int index, const QVariant &value)
    {
        this->addValue(index, value);
    }

    /*
     * Convert to string
     */
    QString CValueMap::convertToQString(bool i18n) const
    {
        if (this->isEmpty()) return "{}";
        QString s;
        foreach(int index, this->m_values.keys())
        {
            QVariant qv = this->m_values.value(index);
            s.isEmpty() ? s.append("{") : s.append(", ");
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
     * Marshall to DBus
     */
    void CValueMap::marshallToDbus(QDBusArgument &argument) const
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
    void CValueMap::unmarshallFromDbus(const QDBusArgument &argument)
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
    void CValueMap::addValue(int index, const QVariant &value)
    {
        this->m_values.insert(index, value);
    }

    /*
     * Register metadata
     */
    void CValueMap::registerMetadata()
    {
        qRegisterMetaType<CValueMap>();
        qDBusRegisterMetaType<CValueMap>();
    }

    /*
     * Hash
     */
    uint CValueMap::getValueHash() const
    {
        return qHash(this);
    }


} // namespace
