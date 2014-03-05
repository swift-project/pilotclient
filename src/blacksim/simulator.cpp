#include "simulator.h"
#include "blackmisc/blackmiscfreefunctions.h"

namespace BlackSim
{

    CSimulator::CSimulator(const QString &shortname, const QString &fullname) : m_fullname(fullname), m_shortname(shortname)
    { }

    CSimulator::CSimulator() :  m_fullname("Unknown"), m_shortname("Unknown")
    {}

    uint CSimulator::getValueHash() const
    {
        QList<uint> hashs;
        hashs << qHash(this->m_fullname);
        hashs << qHash(this->m_shortname);
        return BlackMisc::calculateHash(hashs, "CSimulator");
    }

    QString CSimulator::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        return QString(this->m_shortname).append(" (").append(this->m_fullname).append(")");
    }

    int CSimulator::getMetaTypeId() const
    {
        return qMetaTypeId<CSimulator>();
    }

    int CSimulator::compareImpl(const BlackMisc::CValueObject &other) const
    {
        const CSimulator &otherObj = static_cast<const CSimulator &>(other);
        int result;

        if ((result = this->m_shortname.compare(otherObj.m_shortname, Qt::CaseInsensitive))) return result;
        return this->m_shortname.compare(otherObj.m_shortname, Qt::CaseInsensitive);
    }

    void CSimulator::marshallToDbus(QDBusArgument &argument) const
    {
        argument << this->m_fullname;
        argument << this->m_shortname;
    }

    void CSimulator::unmarshallFromDbus(const QDBusArgument &argument)
    {
        argument >> this->m_fullname;
        argument >> this->m_shortname;
    }

    /*
     * Register metadata
     */
    void CSimulator::registerMetadata()
    {
        qRegisterMetaType<CSimulator>();
        qDBusRegisterMetaType<CSimulator>();
    }
}
