#include "aircraftview.h"
#include <QHeaderView>

using namespace BlackMisc;

namespace BlackGui
{
    CAircraftView::CAircraftView(QWidget *parent) : CViewBase(parent)
    {
        this->m_model = new CAircraftListModel(this);
        this->setModel(this->m_model); // via QTableView
        this->m_model->setSortColumnByPropertyIndex(BlackMisc::Aviation::CAircraft::IndexDistance);
        if (this->m_model->hasValidSortColumn())
            this->horizontalHeader()->setSortIndicator(
                this->m_model->getSortColumn(),
                this->m_model->getSortOrder());
    }
}
