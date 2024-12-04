// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_COCKPITCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_COCKPITCOMPONENT_H

#include <QFrame>
#include <QObject>
#include <QScopedPointer>
#include <QSize>

#include "gui/overlaymessagesframe.h"
#include "gui/swiftguiexport.h"

namespace Ui
{
    class CCockpitComponent;
}
namespace swift::gui
{
    class CDockWidgetInfoArea;

    namespace components
    {
        //! Cockpit component: COM unit, show / hide bar, voice rooms
        class SWIFT_GUI_EXPORT CCockpitComponent : public COverlayMessagesFrameEnableForDockWidgetInfoArea
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CCockpitComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CCockpitComponent() override;

            //! \copydoc CEnableForDockWidgetInfoArea::setParentDockWidgetInfoArea
            virtual bool setParentDockWidgetInfoArea(swift::gui::CDockWidgetInfoArea *parentDockableWidget) override;

            //! Is the info area shown?
            bool isInfoAreaShown() const;

            //! \copydoc swift::gui::components::CTransponderModeSelector::setSelectedTransponderModeStateIdent
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
    } // namespace components
} // namespace swift::gui

#endif // SWIFT_GUI_COMPONENTS_COCKPITCOMPONENT_H
