#include "userlistmodel.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include <QMetaProperty>
#include <QBrush>

using namespace BlackMisc::Network;

namespace BlackGui
{
    /*
     * Constructor
     */
    CUserListModel::CUserListModel(QObject *parent) :
        CListModelBase<BlackMisc::Network::CUser, BlackMisc::Network::CUserList>("ViewUserList", parent)
    {
        this->m_columns.addColumn(CUser::IndexRealName, "realname");
        this->m_columns.addColumn(CUser::IndexCallsign, "callsign");
        this->m_columns.addColumn(CUser::IndexId, "userid");
        this->m_columns.addColumn(CUser::IndexEmail, "email");

        // force strings for translation in resource files
        (void)QT_TRANSLATE_NOOP("ViewUserList", "callsign");
        (void)QT_TRANSLATE_NOOP("ViewUserList", "realname");
        (void)QT_TRANSLATE_NOOP("ViewUserList", "userid");
        (void)QT_TRANSLATE_NOOP("ViewUserList", "email");
    }
}
