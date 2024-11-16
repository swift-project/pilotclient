// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_INFOBARSTATUSCOMPONENT_H
#define SWIFT_GUI_INFOBARSTATUSCOMPONENT_H

#include <QFrame>
#include <QObject>
#include <QScopedPointer>
#include <QString>

#include "core/actionbind.h"
#include "gui/swiftguiexport.h"
#include "misc/audio/audiodeviceinfo.h"
#include "misc/input/actionhotkeydefs.h"
#include "misc/network/connectionstatus.h"

class QPoint;

namespace Ui
{
    class CInfoBarStatusComponent;
}
namespace swift::gui::components
{
    //! Info bar displaying status (Network, Simulator, DBus)
    class SWIFT_GUI_EXPORT CInfoBarStatusComponent : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CInfoBarStatusComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CInfoBarStatusComponent() override;

        //! DBus used?
        void setDBusStatus(bool dbus);

        //! Tooltip for DBus
        void setDBusTooltip(const QString &tooltip);

        //! Set the spacing
        void setSpacing(int spacing);

    signals:
        //! Mode changed
        void transponderModeChanged();

    protected:
        //! \copydoc QWidget::resizeEvent
        virtual void resizeEvent(QResizeEvent *event) override;

    private:
        QScopedPointer<Ui::CInfoBarStatusComponent> ui;
        swift::core::CActionBind m_actionPtt { swift::misc::input::pttHotkeyAction(), swift::misc::input::pttHotkeyIcon(), this, &CInfoBarStatusComponent::onPttChanged };

        swift::misc::CDigestSignal m_dsResize { this, &CInfoBarStatusComponent::adjustTextSize, 1000, 50 };

        //! Audio available and NOT muted
        static bool isAudioAvailableAndNotMuted();

        //! Init the LEDs
        void initLeds();

        //! Init labels
        void adjustTextSize();

        //! Simulator connection has been changed
        void onSimulatorStatusChanged(int status);

        //! Network connection has been changed
        void onNetworkConnectionChanged(const swift::misc::network::CConnectionStatus &from, const swift::misc::network::CConnectionStatus &to);

        //! Context menu requested
        void onCustomAudioContextMenuRequested(const QPoint &position);

        //! Output mute changed
        void onOutputMuteChanged(bool muted);

        //! Audio started
        void onAudioStarted(const swift::misc::audio::CAudioDeviceInfo &input, const swift::misc::audio::CAudioDeviceInfo &output);

        //! Audio stopped
        void onAudioStopped();

        //! Mapper is ready
        void onMapperReady();

        //! PTT button changed
        void onPttChanged(bool enabled);

        //! Update values
        void updateValues();

        //! Update spacing based on main window size
        void updateSpacing();
    };
} // namespace swift::gui::components

#endif // guard
