#include "remote_aircraft.h"

CRemoteAircraft::CRemoteAircraft() :
    m_callsign(), m_heading(0), m_groundSpeed(0), m_wakeTurbulence()
{
}

CRemoteAircraft::CRemoteAircraft(const CRemoteAircraft &other)  :
    m_callsign(other.m_callsign),
    m_heading(other.m_heading),
    m_groundSpeed(other.m_groundSpeed),
    m_wakeTurbulence(other.m_wakeTurbulence)
{
}

CRemoteAircraft &CRemoteAircraft::operator =(const CRemoteAircraft &other)
{
    if (this != &other)
    {
        m_callsign = other.m_callsign;
        m_heading = other.m_heading;
        m_groundSpeed = other.m_groundSpeed;
        m_wakeTurbulence = other.m_wakeTurbulence;
    }

    return *this;
}

void CRemoteAircraft::registerMetaType()
{
    qRegisterMetaType<CRemoteAircraft>("CRemoteAircraft");
    qDBusRegisterMetaType<CRemoteAircraft>();
    qDBusRegisterMetaType<CRemoteAircraftList>();
}

QDBusArgument &operator<<(QDBusArgument &argument, const CRemoteAircraft& remoteAircraft)
{
    argument.beginStructure();
    argument << remoteAircraft.m_callsign;
    argument << remoteAircraft.m_heading;
    argument << remoteAircraft.m_groundSpeed;
    argument << remoteAircraft.m_wakeTurbulence;
    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, CRemoteAircraft &remoteAircraft)
{
    argument.beginStructure();
    argument >> remoteAircraft.m_callsign;
    argument >> remoteAircraft.m_heading;
    argument >> remoteAircraft.m_groundSpeed;
    argument >> remoteAircraft.m_wakeTurbulence;
    argument.endStructure();

    return argument;
}
