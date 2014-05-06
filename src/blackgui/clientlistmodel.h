#ifndef BLACKGUI_CLIENTLISTMODEL_H
#define BLACKGUI_CLIENTLISTMODEL_H

#include <QAbstractItemModel>
#include "blackmisc/nwclientlist.h"
#include "blackgui/listmodelbase.h"

namespace BlackGui
{
    /*!
     * \brief Server list model
     */
    class CClientListModel : public CListModelBase<BlackMisc::Network::CClient, BlackMisc::Network::CClientList>
    {

    public:

        //! \brief Constructor
        explicit CClientListModel(QObject *parent = nullptr);

        //! \brief Destructor
        virtual ~CClientListModel() {}

        //! \copydoc CListModelBase::data
        QVariant data(const QModelIndex &modelIndex, int role = Qt::DisplayRole) const;

    };
}
#endif // guard
