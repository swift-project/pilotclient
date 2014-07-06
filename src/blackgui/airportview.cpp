#include "airportview.h"
#include <QHeaderView>

using namespace BlackMisc;

namespace BlackGui
{
    CAirportView::CAirportView(QWidget *parent) : CViewBase(parent)
    {
        this->m_model = new CAirportListModel(this);
        this->setModel(this->m_model); // via QTableView
        this->m_model->setSortColumnByPropertyIndex(BlackMisc::Aviation::CAirport::IndexDistance);
        if (this->m_model->hasValidSortColumn())
            this->horizontalHeader()->setSortIndicator(
                this->m_model->getSortColumn(),
                this->m_model->getSortOrder());
    }
}
