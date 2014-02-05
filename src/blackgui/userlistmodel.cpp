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
        this->m_columns.addColumn(CUser::IndexCallsignIcon, "");
        this->m_columns.addColumn(CUser::IndexRealName, "realname");
        this->m_columns.addColumn(CUser::IndexCallsign, "callsign");
        this->m_columns.addColumn(CUser::IndexId, "userid");
        // this->m_columns.addColumn(CUser::IndexEmail, "email");

        // force strings for translation in resource files
        (void)QT_TRANSLATE_NOOP("ViewUserList", "callsign");
        (void)QT_TRANSLATE_NOOP("ViewUserList", "realname");
        (void)QT_TRANSLATE_NOOP("ViewUserList", "userid");
        (void)QT_TRANSLATE_NOOP("ViewUserList", "email");
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
