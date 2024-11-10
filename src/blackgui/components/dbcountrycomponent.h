// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_DBCOUNTRYCOMPONENT_H
#define BLACKGUI_COMPONENTS_DBCOUNTRYCOMPONENT_H

#include "blackgui/enablefordockwidgetinfoarea.h"
#include "blackgui/enableforviewbasedindicator.h"
#include "blackgui/blackguiexport.h"
#include "misc/network/entityflags.h"

#include <QFrame>
#include <QObject>
#include <QScopedPointer>

class QWidget;

namespace Ui
{
    class CDbCountryComponent;
}
namespace BlackGui::Components
{
    /*!
     * The countries
     */
    class BLACKGUI_EXPORT CDbCountryComponent :
        public QFrame,
        public CEnableForDockWidgetInfoArea,
        public BlackGui::CEnableForViewBasedIndicator
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CDbCountryComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CDbCountryComponent() override;

    private:
        //! Countries have been read
        void onCountriesRead(swift::misc::network::CEntityFlags::Entity entity, swift::misc::network::CEntityFlags::ReadState readState, int count, const QUrl &url);

        //! Reload models
        void onReload();

    private:
        QScopedPointer<Ui::CDbCountryComponent> ui;
    };

} // ns

#endif // guard
