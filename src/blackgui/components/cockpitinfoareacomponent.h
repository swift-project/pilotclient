/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COCKPITINFOAREACOMPONENT_H
#define BLACKGUI_COCKPITINFOAREACOMPONENT_H

#include "../infoarea.h"
#include <QMainWindow>
#include <QScopedPointer>

namespace Ui { class CCockpitInfoAreaComponent; }
namespace BlackGui
{
    namespace Components
    {
        //! The cockpit itself is part of the main info area, but itself also an info area.
        //! hence windows can be docked in the cockpit too.
        class CCockpitInfoAreaComponent : public CInfoArea
        {
            Q_OBJECT

        public:
            //! Destructor
            explicit CCockpitInfoAreaComponent(QWidget *parent = nullptr);

            //! Destructor
            ~CCockpitInfoAreaComponent();

        public slots:
            //! CInfoArea::toggleFloating
            virtual void toggleFloating() override;

        protected:
            //! \copydoc CInfoArea::getPreferredSizeWhenFloating
            virtual QSize getPreferredSizeWhenFloating(int areaIndex) const override;

            //! \copydoc CInfoArea::indexToPixmap
            virtual const QPixmap &indexToPixmap(int areaIndex) const override;

        private:
            QScopedPointer<Ui::CCockpitInfoAreaComponent> ui;
        };

    } // namespace
} // namespace
#endif // guard
