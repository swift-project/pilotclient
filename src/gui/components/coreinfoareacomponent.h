// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_COREINFOAREACOMPONENT_H
#define SWIFT_GUI_COMPONENTS_COREINFOAREACOMPONENT_H

#include <QObject>
#include <QScopedPointer>
#include <QSize>

#include "gui/infoarea.h"
#include "gui/swiftguiexport.h"

class QPixmap;
class QWidget;

namespace Ui
{
    class CCoreInfoAreaComponent;
}

namespace swift::gui::components
{
    class CCoreStatusComponent;
    class CLogComponent;

    //! Main info area
    class SWIFT_GUI_EXPORT CCoreInfoAreaComponent : public swift::gui::CInfoArea
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CCoreInfoAreaComponent(QWidget *parent = nullptr);

        //! Destructor
        ~CCoreInfoAreaComponent() override;

        //! Info areas
        enum InfoArea
        {
            // index must match tab index!
            InfoAreaLog = 0,
            InfoAreaStatus = 1,
            InfoAreaNone = -1
        };

        //! Log messages
        CLogComponent *getLogComponent();

        //! Simulator
        CCoreStatusComponent *getStatusComponent();

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
        QScopedPointer<Ui::CCoreInfoAreaComponent> ui;
    };
} // namespace swift::gui::components

#endif // SWIFT_GUI_COMPONENTS_COREINFOAREACOMPONENT_H
