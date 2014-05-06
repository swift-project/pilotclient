#ifndef BLACKGUI_CLIENTVIEW_H
#define BLACKGUI_CLIENTVIEW_H

#include "viewbase.h"
#include "clientlistmodel.h"

namespace BlackGui
{
    /*!
     * \brief Client view
     */
    class CClientView : public CViewBase<CClientListModel>
    {

    public:

        //! Constructor
        explicit CClientView(QWidget *parent = nullptr);
    };
}
#endif // guard
