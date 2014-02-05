#ifndef BLACKGUI_USERLISTMODEL_H
#define BLACKGUI_USERLISTMODEL_H

#include <QAbstractItemModel>
#include <QDBusConnection>
#include "blackmisc/nwuserlist.h"
#include "blackgui/listmodelbase.h"

namespace BlackGui
{
    /*!
     * \brief Server list model
     */
    class CUserListModel : public CListModelBase<BlackMisc::Network::CUser, BlackMisc::Network::CUserList>
    {

    public:

        //! \brief Constructor
        explicit CUserListModel(QObject *parent = nullptr);

        //! \brief Destructor
        virtual ~CUserListModel() {}

        //! \copydoc CListModelBase::data
        QVariant data(const QModelIndex &modelIndex, int role = Qt::DisplayRole) const;
    };
}
#endif // guard
