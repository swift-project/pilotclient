#ifndef BLACKGUI_STATUSMESSAGEVIEW_H
#define BLACKGUI_STATUSMESSAGEVIEW_H

#include "viewbase.h"
#include "statusmessagelistmodel.h"

namespace BlackGui
{
    /*!
     * \brief Status message view
     */
    class CStatusMessageView : public CViewBase<CStatusMessageListModel>
    {

    public:

        //! Constructor
        explicit CStatusMessageView(QWidget *parent = nullptr);

    private:
        QMenu *m_contextMenu;

    private slots:
        //! Context menu for message list
        void contextMenu(const QPoint &position);
    };
}
#endif // guard
