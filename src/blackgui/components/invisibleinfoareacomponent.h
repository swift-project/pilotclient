/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKGUI_COMPONENTS_INVISIBLEINFOAREACOMPONENT_H
#define BLACKGUI_COMPONENTS_INVISIBLEINFOAREACOMPONENT_H

#include "blackgui/infoarea.h"
#include <QMainWindow>

namespace Ui { class CInvisibleInfoAreaComponent; }

namespace BlackGui
{
    namespace Components
    {

        //! Info area to carry info areas on visibile when floating
        class CInvisibleInfoAreaComponent : public BlackGui::CInfoArea
        {
            Q_OBJECT

        public:

            //! Info areas
            enum InfoArea
            {
                // index must match tab index!
                InfoAreaVerticalNavigator   =  0,
                InfoAreaHorizontalNavigator =  1
            };

            //! Consturctor
            explicit CInvisibleInfoAreaComponent(QWidget *parent = nullptr);

            //! Destructor
            ~CInvisibleInfoAreaComponent();

            //! \copydoc CInfoArea::getPreferredSizeWhenFloating
            virtual QSize getPreferredSizeWhenFloating(int areaIndex) const override;

            //! \copydoc CInfoArea::indexToPixmap
            const QPixmap &indexToPixmap(int areaIndex) const override;

        public slots:
            //! Navigator floating
            void toggleNavigator();

        private:
            QScopedPointer<Ui::CInvisibleInfoAreaComponent> ui;
        };

    } // ns
} // ns


#endif // guard
