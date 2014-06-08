#include "clientlistmodel.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include <QMetaProperty>
#include <QBrush>

using namespace BlackMisc::Network;

namespace BlackGui
{
    /*
     * Constructor
     */
    CClientListModel::CClientListModel(QObject *parent) :
        CListModelBase<BlackMisc::Network::CClient, BlackMisc::Network::CClientList>("ViewClientList", parent)
    {
        this->m_columns.addColumn(CColumn("", CClient::IndexCallsignIcon));
        this->m_columns.addColumn(CColumn("callsign", CClient::IndexCallsign));
        this->m_columns.addColumn(CColumn("realname", CClient::IndexRealName));
        this->m_columns.addColumn(CColumn("", CClient::IndexVoiceCapabilitiesIcon));
        this->m_columns.addColumn(CColumn("capabilities", CClient::IndexCapabilitiesString));
        this->m_columns.addColumn(CColumn("model", CClient::IndexQueriedModelString));
        this->m_columns.addColumn(CColumn("host", CClient::IndexHost));

        // force strings for translation in resource files
        // force strings for translation in resource files
        (void)QT_TRANSLATE_NOOP("ViewClientList", "callsign");
        (void)QT_TRANSLATE_NOOP("ViewClientList", "realname");
        (void)QT_TRANSLATE_NOOP("ViewClientList", "userid");
        (void)QT_TRANSLATE_NOOP("ViewClientList", "model");
        (void)QT_TRANSLATE_NOOP("ViewClientList", "host");
    }

    /*
     * Display icons
     */
    QVariant CClientListModel::data(const QModelIndex &modelIndex, int role) const
    {
        // shortcut, fast check
        if (role != Qt::DecorationRole) return CListModelBase::data(modelIndex, role);
        if (this->columnToPropertyIndex(modelIndex.column()) == CClient::IndexCallsignIcon)
        {
            if (role == Qt::DecorationRole)
            {
                CClient u = this->at(modelIndex);
                return u.toIcon();
            }
        }
        return CListModelBase::data(modelIndex, role);
    }
}
