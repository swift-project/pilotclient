#ifndef BLACKGUI_STATUSMESSAGELISTMODEL_H
#define BLACKGUI_STATUSMESSAGELISTMODEL_H

#include <QAbstractItemModel>
#include <QDBusConnection>
#include "blackmisc/statusmessagelist.h"
#include "blackgui/listmodelbase.h"

namespace BlackGui
{
    /*!
     * \brief Server list model
     */
    class CStatusMessageListModel : public CListModelBase<BlackMisc::CStatusMessage, BlackMisc::CStatusMessageList>
    {

    public:

        //! \brief Constructor
        explicit CStatusMessageListModel(QObject *parent = nullptr);

        //! \brief Destructor
        virtual ~CStatusMessageListModel() {}

        //! \copydoc CListModelBase::data
        QVariant data(const QModelIndex &modelIndex, int role = Qt::DisplayRole) const;

    };
}
#endif // guard
