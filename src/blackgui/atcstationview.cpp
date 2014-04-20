#include "atcstationview.h"
#include <QHeaderView>

using namespace BlackMisc;

namespace BlackGui
{
    CAtcStationView::CAtcStationView(QWidget *parent) : CViewBase(parent)
    {
        this->m_model = new CAtcStationListModel(CAtcStationListModel::StationsOnline, this);
        this->setModel(this->m_model); // via QTableView
        this->m_model->setSortColumnByPropertyIndex(BlackMisc::Aviation::CAtcStation::IndexDistance);
        if (this->m_model->hasValidSortColumn())
            this->horizontalHeader()->setSortIndicator(
                this->m_model->getSortColumn(),
                this->m_model->getSortOrder());
    }

    void CAtcStationView::setStationMode(CAtcStationListModel::AtcStationMode stationMode)
    {
        Q_ASSERT(this->m_model);
        this->m_model->setStationMode(stationMode);
    }
}
