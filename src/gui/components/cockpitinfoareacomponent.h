// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COCKPITINFOAREACOMPONENT_H
#define SWIFT_GUI_COCKPITINFOAREACOMPONENT_H

#include <QObject>
#include <QScopedPointer>
#include <QSize>

#include "gui/infoarea.h"
#include "gui/swiftguiexport.h"

class QPixmap;
class QWidget;

namespace Ui
{
    class CCockpitInfoAreaComponent;
}
namespace swift::gui::components
{
    //! The cockpit itself is part of the main info area, but itself also an info area.
    //! hence windows can be docked in the cockpit too.
    class SWIFT_GUI_EXPORT CCockpitInfoAreaComponent : public swift::gui::CInfoArea
    {
        Q_OBJECT

    public:
        //! Destructor
        explicit CCockpitInfoAreaComponent(QWidget *parent = nullptr);

        //! Destructor
        ~CCockpitInfoAreaComponent() override;

        //! Info areas
        enum InfoArea
        {
            // index must match tab index!
            InfoAreaNotifications = 0,
            InfoAreaAudio = 1,
            InfoAreaAdvanced = 2,
            InfoAreaNone = -1
        };

    public slots:
        //! Toggle floating of given area
        void toggleFloating(InfoArea infoArea) { CInfoArea::toggleFloatingByIndex(static_cast<int>(infoArea)); }

        //! Select area
        void selectArea(InfoArea infoArea) { CInfoArea::selectArea(static_cast<int>(infoArea)); }

    protected:
        //! \copydoc CInfoArea::getPreferredSizeWhenFloating
        QSize getPreferredSizeWhenFloating(int areaIndex) const override;

        //! \copydoc CInfoArea::indexToPixmap
        const QPixmap &indexToPixmap(int areaIndex) const override;

    private:
        QScopedPointer<Ui::CCockpitInfoAreaComponent> ui;
    };

} // namespace swift::gui::components
#endif // SWIFT_GUI_COCKPITINFOAREACOMPONENT_H
