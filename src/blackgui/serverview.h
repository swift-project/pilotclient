#ifndef BLACKGUI_SERVERVIEW_H
#define BLACKGUI_SERVERVIEW_H

#include "viewbase.h"
#include "serverlistmodel.h"

namespace BlackGui
{
    /*!
     * \brief Network servers
     */
    class CServerView : public CViewBase<CServerListModel>
    {

    public:

        //! Constructor
        explicit CServerView(QWidget *parent = nullptr);

        //! \copydoc CServerListModel::setSelectedServer
        void setSelectedServer(const BlackMisc::Network::CServer &selectedServer);
    };
}
#endif // guard
