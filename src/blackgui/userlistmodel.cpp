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
    CUserListModel::CUserListModel(UserMode userMode, QObject *parent) :
        CListModelBase<BlackMisc::Network::CUser, BlackMisc::Network::CUserList>("ViewUserList", parent), m_userMode(NotSet)
    {
        this->setUserMode(userMode);
        // force strings for translation in resource files
        (void)QT_TRANSLATE_NOOP("ViewUserList", "callsign");
        (void)QT_TRANSLATE_NOOP("ViewUserList", "realname");
        (void)QT_TRANSLATE_NOOP("ViewUserList", "userid");
        (void)QT_TRANSLATE_NOOP("ViewUserList", "email");
    }

    /*
     * Mode
     */
    void CUserListModel::setUserMode(CUserListModel::UserMode userMode)
    {
        if (this->m_userMode == userMode) return;
        this->m_userMode = userMode;
        this->m_columns.clear();
        switch (userMode)
        {
        case UserDetailed:
            this->m_columns.addColumn(CColumn("", CUser::IndexCallsignIcon));
            this->m_columns.addColumn(CColumn("realname", CUser::IndexRealName));
            this->m_columns.addColumn(CColumn("callsign", CUser::IndexCallsign));
            this->m_columns.addColumn(CColumn("userid", CUser::IndexId));
            // this->m_columns.addColumn(CUser::IndexEmail, "email");
            break;

        case UserShort:
            this->m_columns.addColumn(CColumn("", CUser::IndexCallsignIcon));
            this->m_columns.addColumn(CColumn("realname", CUser::IndexRealName));
            this->m_columns.addColumn(CColumn("callsign", CUser::IndexCallsign));
            break;
        }
    }

    /*
     * Display icons
     */
    QVariant CUserListModel::data(const QModelIndex &modelIndex, int role) const
    {
        // shortcut, fast check
        if (role != Qt::DecorationRole) return CListModelBase::data(modelIndex, role);
        if (this->columnToPropertyIndex(modelIndex.column()) == CUser::IndexCallsignIcon)
        {
            if (role == Qt::DecorationRole)
            {
                CUser u = this->at(modelIndex);
                return u.toIcon();
            }
        }
        return CListModelBase::data(modelIndex, role);
    }
}
