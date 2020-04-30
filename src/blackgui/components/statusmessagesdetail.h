/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_STATUSMESSAGESDETAIL_H
#define BLACKGUI_COMPONENTS_STATUSMESSAGESDETAIL_H

#include "blackgui/menus/menudelegate.h"
#include "blackgui/blackguiexport.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/digestsignal.h"
#include "blackmisc/variant.h"

#include <QFrame>
#include <QObject>
#include <QScopedPointer>

namespace Ui { class CStatusMessagesDetail;  }
namespace BlackGui
{
    namespace Components
    {
        //! Status messages plus detail
        class BLACKGUI_EXPORT CStatusMessagesDetail : public QFrame
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CStatusMessagesDetail(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CStatusMessagesDetail() override;

            //! Add message
            void appendStatusMessageToList(const BlackMisc::CStatusMessage &message);

            //! Add messages
            void appendStatusMessagesToList(const BlackMisc::CStatusMessageList &messages);

            //! Show log details
            void showDetails(bool details);

            //! Show filter dialog and disable bar
            void showFilterDialog();

            //! Show a filter bar
            void showFilterBar();

            //! Hide a filter bar
            void hideFilterBar();

            //! Clear
            void clear();

            //! Number of status messages in view
            int rowCount() const;

            //! Set max.log messages
            void setMaxLogMessages(int desiredNumber) { m_maxLogMessages = desiredNumber; }

            //! \copydoc BlackGui::Filters::CStatusMessageFilterBar::useRadioButtonDescriptiveIcons
            void filterUseRadioButtonDescriptiveIcons(bool oneCharacterText);

            //! Sorting for view
            void setSorting(const BlackMisc::CPropertyIndex &propertyIndex, Qt::SortOrder order = Qt::AscendingOrder);

            //! Disable sorting
            void setNoSorting();

        signals:
            //! \copydoc BlackGui::Views::CStatusMessageView::modelDataChangedDigest
            void modelDataChangedDigest(int count, bool withFilter);

            //! The user changed their message filter
            void filterChanged(const BlackMisc::CVariant &filter);

        private:
            QScopedPointer<Ui::CStatusMessagesDetail> ui;
            int m_maxLogMessages = -1;
            BlackMisc::CStatusMessageList m_pending; //!< pending messages which will be added with next CStatusMessagesDetail::deferredUpdate
            BlackMisc::CDigestSignal      m_dsDeferredUpdate { this, &CStatusMessagesDetail::deferredUpdate, 2000, 25 };

            //! Do not update each message, but deferred
            void deferredUpdate();

            //! Custom menu for the log component
            //! \fixme Move to namespace scope and add Q_OBJECT
            class CMessageMenu : public Menus::IMenuDelegate
            {
            public:
                //! Constructor
                CMessageMenu(CStatusMessagesDetail *parent) : IMenuDelegate(parent) {}

                //! \copydoc IMenuDelegate::customMenu
                virtual void customMenu(Menus::CMenuActions &menuActions) override;

            private:
                QAction *m_action = nullptr;
            };
        };
    } // ns
} // ns

#endif // guard
