/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COCKPITINFOAREACOMPONENT_H
#define BLACKGUI_COCKPITINFOAREACOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "blackgui/infoarea.h"

#include <QObject>
#include <QScopedPointer>
#include <QSize>

class QPixmap;
class QWidget;

namespace Ui { class CCockpitInfoAreaComponent; }
namespace BlackGui::Components
{
    //! The cockpit itself is part of the main info area, but itself also an info area.
    //! hence windows can be docked in the cockpit too.
    class BLACKGUI_EXPORT CCockpitInfoAreaComponent : public BlackGui::CInfoArea
    {
        Q_OBJECT

    public:
        //! Destructor
        explicit CCockpitInfoAreaComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CCockpitInfoAreaComponent() override;

        //! Info areas
        enum InfoArea
        {
            // index must match tab index!
            InfoAreaNotifications =  0,
            InfoAreaAudio         =  1,
            InfoAreaAdvanced      =  2,
            InfoAreaNone          = -1
        };

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
        QScopedPointer<Ui::CCockpitInfoAreaComponent> ui;
    };

} // namespace
#endif // guard
