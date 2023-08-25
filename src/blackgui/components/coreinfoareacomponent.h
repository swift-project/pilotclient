// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_COREINFOAREACOMPONENT_H
#define BLACKGUI_COMPONENTS_COREINFOAREACOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "blackgui/infoarea.h"

#include <QObject>
#include <QScopedPointer>
#include <QSize>

class QPixmap;
class QWidget;

namespace Ui
{
    class CCoreInfoAreaComponent;
}

namespace BlackGui::Components
{
    class CCoreStatusComponent;
    class CLogComponent;

    //! Main info area
    class BLACKGUI_EXPORT CCoreInfoAreaComponent : public BlackGui::CInfoArea
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CCoreInfoAreaComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CCoreInfoAreaComponent() override;

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
        virtual QSize getPreferredSizeWhenFloating(int areaIndex) const override;

        //! \copydoc CInfoArea::indexToPixmap
        virtual const QPixmap &indexToPixmap(int areaIndex) const override;

    private:
        QScopedPointer<Ui::CCoreInfoAreaComponent> ui;
    };
} // ns

#endif // guard
