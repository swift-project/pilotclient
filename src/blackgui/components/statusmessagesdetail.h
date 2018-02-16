/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_STATUSMESSAGESDETAIL_H
#define BLACKGUI_COMPONENTS_STATUSMESSAGESDETAIL_H

#include "blackgui/menus/menudelegate.h"
#include "blackgui/blackguiexport.h"
#include "blackmisc/statusmessagelist.h"
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
            virtual ~CStatusMessagesDetail();

            //! Add message
            void appendStatusMessageToList(const BlackMisc::CStatusMessage &message, bool resize = true);

            //! Add messages
            void appendStatusMessagesToList(const BlackMisc::CStatusMessageList &messages, bool resize = true);

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

        signals:
            //! \copydoc BlackGui::Views::CStatusMessageView::modelDataChangedDigest
            void modelDataChangedDigest(int count, bool withFilter);

        private:
            QScopedPointer<Ui::CStatusMessagesDetail> ui;
            int m_maxLogMessages = -1;

            //! Remove oldest messages
            void removeOldest();

            //! Custom menu for the log component
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
