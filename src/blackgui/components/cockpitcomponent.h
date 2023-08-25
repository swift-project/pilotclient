// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_COCKPITCOMPONENT_H
#define BLACKGUI_COMPONENTS_COCKPITCOMPONENT_H

#include "blackgui/overlaymessagesframe.h"
#include "blackgui/blackguiexport.h"

#include <QFrame>
#include <QObject>
#include <QScopedPointer>
#include <QSize>

namespace Ui
{
    class CCockpitComponent;
}
namespace BlackGui
{
    class CDockWidgetInfoArea;

    namespace Components
    {
        //! Cockpit component: COM unit, show / hide bar, voice rooms
        class BLACKGUI_EXPORT CCockpitComponent : public COverlayMessagesFrameEnableForDockWidgetInfoArea
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CCockpitComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CCockpitComponent() override;

            //! \copydoc CEnableForDockWidgetInfoArea::setParentDockWidgetInfoArea
            virtual bool setParentDockWidgetInfoArea(BlackGui::CDockWidgetInfoArea *parentDockableWidget) override;

            //! Is the info area shown?
            bool isInfoAreaShown() const;

            //! \copydoc BlackGui::Components::CTransponderModeSelector::setSelectedTransponderModeStateIdent
            void setSelectedTransponderModeStateIdent();

            //! Show the audio UI
            void showAudio();

        protected:
            //! \copydoc QWidget::mouseDoubleClickEvent
            virtual void mouseDoubleClickEvent(QMouseEvent *event) override;

        private:
            //! Show or hide cockpit details
            void onToggleShowHideDetails(bool show);

            //! Toggle floating
            void onToggleFloating(bool floating);

            //! Toggle area on show/hide details
            void toggleShowHideDetails(bool show, bool considerCurrentSize);

            //! @{
            //! Request text message COM1
            void onRequestTextMessageCom1();
            void onRequestTextMessageCom2();
            //! @}

            //! ATC stations have been changed
            void onATCStationsChanged();

            QScopedPointer<Ui::CCockpitComponent> ui;
            QSize m_sizeFloatingShown; //! size when info area is shown
            QSize m_sizeFloatingHidden; //! size when info area is hidden
            int m_minHeightInfoArea = -1; //! minimum height of the info area
        };
    } // namespace
} // namespace

#endif // guard
