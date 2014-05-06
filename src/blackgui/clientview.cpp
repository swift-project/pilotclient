#include "clientview.h"
#include <QHeaderView>

using namespace BlackMisc;

namespace BlackGui
{
    CClientView::CClientView(QWidget *parent) : CViewBase(parent)
    {
        this->m_model = new CClientListModel(this);
        this->setModel(this->m_model); // via QTableView
        this->m_model->setSortColumnByPropertyIndex(BlackMisc::Network::CClient::IndexRealName);
        if (this->m_model->hasValidSortColumn())
            this->horizontalHeader()->setSortIndicator(
                this->m_model->getSortColumn(),
                this->m_model->getSortOrder());
        this->horizontalHeader()->setStretchLastSection(true);
    }
}
