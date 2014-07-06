#ifndef BLACKGUI_AIRPORTVIEW_H
#define BLACKGUI_AIRPORTVIEW_H

#include "viewbase.h"
#include "airportlistmodel.h"

namespace BlackGui
{
    /*!
     * Airports view
     */
    class CAirportView : public CViewBase<CAirportListModel>
    {

    public:

        //! Constructor
        explicit CAirportView(QWidget *parent = nullptr);
    };
}
#endif // guard
