#ifndef BLACKGUI_AIRPORTLISTMODEL_H
#define BLACKGUI_AIRPORTLISTMODEL_H

#include <QAbstractItemModel>
#include "blackmisc/avairportlist.h"
#include "blackgui/listmodelbase.h"

namespace BlackGui
{
    /*!
     * Airport list model
     */
    class CAirportListModel : public CListModelBase<BlackMisc::Aviation::CAirport, BlackMisc::Aviation::CAirportList>
    {

    public:

        //! Constructor
        explicit CAirportListModel(QObject *parent = nullptr);

        //! Destructor
        virtual ~CAirportListModel() {}
    };
}
#endif // guard
