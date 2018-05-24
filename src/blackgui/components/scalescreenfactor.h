/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_SCALESCREENFACTOR_H
#define BLACKGUI_COMPONENTS_SCALESCREENFACTOR_H

#include "blackgui/blackguiexport.h"
#include <QFrame>
#include <QScopedPointer>

namespace Ui { class CScaleScreenFactor; }
namespace BlackGui
{
    namespace Components
    {
        /**
         * UI to scale screen factor
         */
        class BLACKGUI_EXPORT CScaleScreenFactor : public QFrame
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CScaleScreenFactor(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CScaleScreenFactor();

            //! Minimum/maximum values
            void setMinMax(int min, int max);

            //! Scale factor
            qreal getScaleFactor() const;

            //! Scale factor as string
            QString getScaleFactorAsString() const;

        private:
            //! Slider value changed
            void onSliderChanged(int value);

            //! Line edit change
            void onEditFinished();

            QScopedPointer<Ui::CScaleScreenFactor> ui;
        };
    } // ns
}// ns

#endif // guard
