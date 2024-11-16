// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_DBCOUNTRYCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_DBCOUNTRYCOMPONENT_H

#include <QFrame>
#include <QObject>
#include <QScopedPointer>

#include "gui/enablefordockwidgetinfoarea.h"
#include "gui/enableforviewbasedindicator.h"
#include "gui/swiftguiexport.h"
#include "misc/network/entityflags.h"

class QWidget;

namespace Ui
{
    class CDbCountryComponent;
}
namespace swift::gui::components
{
    /*!
     * The countries
     */
    class SWIFT_GUI_EXPORT CDbCountryComponent :
        public QFrame,
        public CEnableForDockWidgetInfoArea,
        public swift::gui::CEnableForViewBasedIndicator
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

} // namespace swift::gui::components

#endif // guard
