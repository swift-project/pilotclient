// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_INFOBARSTATUSCOMPONENT_H
#define BLACKGUI_INFOBARSTATUSCOMPONENT_H

#include "blackcore/actionbind.h"
#include "blackgui/blackguiexport.h"

#include "blackmisc/audio/ptt.h"
#include "blackmisc/audio/audiodeviceinfo.h"
#include "blackmisc/input/actionhotkeydefs.h"
#include "blackmisc/network/connectionstatus.h"

#include <QFrame>
#include <QObject>
#include <QScopedPointer>
#include <QString>

class QPoint;

namespace Ui
{
    class CInfoBarStatusComponent;
}
namespace BlackGui::Components
{
    //! Info bar displaying status (Network, Simulator, DBus)
    class BLACKGUI_EXPORT CInfoBarStatusComponent : public QFrame
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
        BlackCore::CActionBind m_actionPtt { BlackMisc::Input::pttHotkeyAction(), BlackMisc::Input::pttHotkeyIcon(), this, &CInfoBarStatusComponent::onPttChanged };

        BlackMisc::CDigestSignal m_dsResize { this, &CInfoBarStatusComponent::adjustTextSize, 1000, 50 };

        //! Audio available and NOT muted
        static bool isAudioAvailableAndNotMuted();

        //! Init the LEDs
        void initLeds();

        //! Init labels
        void adjustTextSize();

        //! Simulator connection has been changed
        void onSimulatorStatusChanged(int status);

        //! Network connection has been changed
        void onNetworkConnectionChanged(const BlackMisc::Network::CConnectionStatus &from, const BlackMisc::Network::CConnectionStatus &to);

        //! Context menu requested
        void onCustomAudioContextMenuRequested(const QPoint &position);

        //! Mute changed
        void onMuteChanged(bool muted);

        //! Audio started
        void onAudioStarted(const BlackMisc::Audio::CAudioDeviceInfo &input, const BlackMisc::Audio::CAudioDeviceInfo &output);

        //! Audio stopped
        void onAudioStopped();

        //! Mapper is ready
        void onMapperReady();

        //! PTT button changed
        void onPttChanged(bool enabled);

        //! PTT, as received in in audio
        void onAudioPtt(bool active, BlackMisc::Audio::PTTCOM pttcom, const BlackMisc::CIdentifier &identifier);

        //! Update values
        void updateValues();

        //! Update spacing based on main window size
        void updateSpacing();
    };
} // ns

#endif // guard
