#include "userview.h"
#include <QHeaderView>

using namespace BlackMisc;

namespace BlackGui
{
    CUserView::CUserView(QWidget *parent) : CViewBase(parent)
    {
        this->m_model = new CUserListModel(CUserListModel::UserDetailed, this);
        this->setModel(this->m_model); // via QTableView
        this->m_model->setSortColumnByPropertyIndex(BlackMisc::Network::CUser::IndexRealName);
        if (this->m_model->hasValidSortColumn())
            this->horizontalHeader()->setSortIndicator(
                this->m_model->getSortColumn(),
                this->m_model->getSortOrder());
        this->horizontalHeader()->setStretchLastSection(true);
    }

    void CUserView::setUserMode(CUserListModel::UserMode userMode)
    {
        Q_ASSERT(this->m_model);
        this->m_model->setUserMode(userMode);
    }
}
