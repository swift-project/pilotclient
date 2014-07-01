#ifndef BLACKGUI_USERLISTMODEL_H
#define BLACKGUI_USERLISTMODEL_H

#include <QAbstractItemModel>
#include "blackmisc/nwuserlist.h"
#include "blackgui/listmodelbase.h"

namespace BlackGui
{
    /*!
     * User list model
     */
    class CUserListModel : public CListModelBase<BlackMisc::Network::CUser, BlackMisc::Network::CUserList>
    {

    public:

        //! What level of detail
        enum UserMode
        {
            NotSet,
            UserDetailed,
            UserShort
        };

        //! Constructor
        explicit CUserListModel(UserMode userMode, QObject *parent = nullptr);

        //! Destructor
        virtual ~CUserListModel() {}

        //! Set station mode
        void setUserMode(UserMode userMode);

    private:
        UserMode m_userMode;
    };
}
#endif // guard
