#include "planemanager.h"

CPlaneManager::CPlaneManager(QObject *parent) :
    QObject(parent)
{
    m_pilotsList << "DLH123" << "BER456" << "SWS789";
}

CPlaneManager::~CPlaneManager()
{
}

QStringList CPlaneManager::pilotList() const
{
    return m_pilotsList;
}
