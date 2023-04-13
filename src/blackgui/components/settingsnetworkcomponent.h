/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_SETTINGSNETWORKCOMPONENT_H
#define BLACKGUI_COMPONENTS_SETTINGSNETWORKCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "blackcore/vatsim/vatsimsettings.h"

#include <QFrame>
#include <QObject>
#include <QScopedPointer>

namespace Ui
{
    class CSettingsNetworkComponent;
}
namespace BlackGui::Components
{
    //! General settings for network
    class BLACKGUI_EXPORT CSettingsNetworkComponent : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CSettingsNetworkComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CSettingsNetworkComponent();

    private:
        QScopedPointer<Ui::CSettingsNetworkComponent> ui;
    };
} // ns

#endif // guard
