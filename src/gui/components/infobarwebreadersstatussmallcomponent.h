// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_INFOBARWEBREADERSSTATUSSMALLCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_INFOBARWEBREADERSSTATUSSMALLCOMPONENT_H

#include "gui/components/infobarwebreadersstatuscomponent.h"
#include <QScopedPointer>

namespace Ui
{
    class CInfoBarWebReadersStatusSmallComponent;
}
namespace swift::gui::components
{
    /*!
     * Smaller version of CInfoBarWebReadersStatusComponent
     */
    class SWIFT_GUI_EXPORT CInfoBarWebReadersStatusSmallComponent : public CInfoBarWebReadersStatusBase
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CInfoBarWebReadersStatusSmallComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CInfoBarWebReadersStatusSmallComponent() override;

    private:
        QScopedPointer<Ui::CInfoBarWebReadersStatusSmallComponent> ui;
    };
} // ns
#endif // guard
