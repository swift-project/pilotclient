#include "atcmanager.h"

CAtcManager::CAtcManager(QObject *parent) :
    QObject(parent)
{
    m_atcList << "EDDM_N_TWR" << "KJFK_GND" << "LOVV_CTR";
}

QStringList CAtcManager::atcList() const
{
    return m_atcList;
}
