#ifndef BLACKGUI_AIRCRAFTVIEW_H
#define BLACKGUI_AIRCRAFTVIEW_H

#include "viewbase.h"
#include "aircraftlistmodel.h"

namespace BlackGui
{
    /*!
     * \brief Aircrafts view
     */
    class CAircraftView : public CViewBase<CAircraftListModel>
    {

    public:

        //! Constructor
        explicit CAircraftView(QWidget *parent = nullptr);
    };
}
#endif // guard
