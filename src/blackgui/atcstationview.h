#ifndef BLACKGUI_ATCSTATIONVIEW_H
#define BLACKGUI_ATCSTATIONVIEW_H

#include "viewbase.h"
#include "atcstationlistmodel.h"

namespace BlackGui
{
    /*!
     * \brief ATC stations view
     */
    class CAtcStationView : public CViewBase<CAtcStationListModel>
    {

    public:

        //! Constructor
        explicit CAtcStationView(QWidget *parent = nullptr);

        //! Set station mode
        void setStationMode(CAtcStationListModel::AtcStationMode stationMode);

    public slots:
        //! \copydoc CAtcStationListModel::changedAtcStationConnectionStatus
        void changedAtcStationConnectionStatus(const BlackMisc::Aviation::CAtcStation &station, bool added);

    };
}
#endif // guard
