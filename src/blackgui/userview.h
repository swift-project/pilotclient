#ifndef BLACKGUI_USERVIEW_H
#define BLACKGUI_USERVIEW_H

#include "viewbase.h"
#include "userlistmodel.h"

namespace BlackGui
{
    /*!
     * \brief User view
     */
    class CUserView : public CViewBase<CUserListModel>
    {

    public:

        //! Constructor
        explicit CUserView(QWidget *parent = nullptr);

        //! Set station mode
        void setUserMode(CUserListModel::UserMode userMode);
    };
}
#endif // guard
