#include "serverview.h"
#include <QHeaderView>

using namespace BlackMisc;

namespace BlackGui
{
    CServerView::CServerView(QWidget *parent) : CViewBase(parent)
    {
        this->m_model = new CServerListModel(this);
        this->setModel(this->m_model); // via QTableView
        this->m_model->setSortColumnByPropertyIndex(BlackMisc::Network::CServer::IndexName);
        if (this->m_model->hasValidSortColumn())
            this->horizontalHeader()->setSortIndicator(
                this->m_model->getSortColumn(),
                this->m_model->getSortOrder());
    }

    void CServerView::setSelectedServer(const Network::CServer &selectedServer)
    {
        Q_ASSERT(this->m_model);
        this->m_model->setSelectedServer(selectedServer);
    }
}
