#include "simulatorinfo.h"
#include "blackmisc/blackmiscfreefunctions.h"

using namespace BlackMisc;

namespace BlackSim
{

    CSimulatorInfo::CSimulatorInfo(const QString &shortname, const QString &fullname) : m_fullname(fullname), m_shortname(shortname)
    { }

    CSimulatorInfo::CSimulatorInfo() :  m_fullname("Unknown"), m_shortname("Unknown")
    {}

    uint CSimulatorInfo::getValueHash() const
    {
        return qHash(TupleConverter<CSimulatorInfo>::toTuple(*this));
    }

    QString CSimulatorInfo::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        return QString(this->m_shortname).append(" (").append(this->m_fullname).append(")");
    }

    int CSimulatorInfo::getMetaTypeId() const
    {
        return qMetaTypeId<CSimulatorInfo>();
    }

    bool CSimulatorInfo::operator ==(const CSimulatorInfo &other) const
    {
        if (this == &other) return true;
        return TupleConverter<CSimulatorInfo>::toTuple(*this) == TupleConverter<CSimulatorInfo>::toTuple(other);
    }

    bool CSimulatorInfo::operator !=(const CSimulatorInfo &other) const
    {
        return !((*this) == other);
    }

    int CSimulatorInfo::compareImpl(const BlackMisc::CValueObject &otherBase) const
    {
        const auto &other = static_cast<const CSimulatorInfo &>(otherBase);
        return compare(TupleConverter<CSimulatorInfo>::toTuple(*this), TupleConverter<CSimulatorInfo>::toTuple(other));
    }

    void CSimulatorInfo::marshallToDbus(QDBusArgument &argument) const
    {
        argument << TupleConverter<CSimulatorInfo>::toTuple(*this);
    }

    void CSimulatorInfo::unmarshallFromDbus(const QDBusArgument &argument)
    {
        argument >> TupleConverter<CSimulatorInfo>::toTuple(*this);
    }

    void CSimulatorInfo::registerMetadata()
    {
        qRegisterMetaType<CSimulatorInfo>();
        qDBusRegisterMetaType<CSimulatorInfo>();
    }
}
