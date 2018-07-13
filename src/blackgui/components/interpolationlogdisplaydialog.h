/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_INTERPOLATIONLOGDISPLAYDIALOG_H
#define BLACKGUI_COMPONENTS_INTERPOLATIONLOGDISPLAYDIALOG_H

#include "blackgui/blackguiexport.h"
#include <QScopedPointer>
#include <QDialog>

namespace BlackCore
{
    class ISimulator;
    class CAirspaceMonitor;
}
namespace Ui { class CInterpolationLogDisplayDialog; }
namespace BlackGui
{
    namespace Components
    {
        /**
         * CInterpolationLogDisplay as dialog
         */
        class BLACKGUI_EXPORT CInterpolationLogDisplayDialog : public QDialog
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CInterpolationLogDisplayDialog(
                BlackCore::ISimulator *simulator, BlackCore::CAirspaceMonitor *airspaceMonitor,
                QWidget *parent = nullptr);

            //! Destructor
            virtual ~CInterpolationLogDisplayDialog();

            //! Set simulator
            void setSimulator(BlackCore::ISimulator *simulator);

            //! Set airspace monitor
            void setAirspaceMonitor(BlackCore::CAirspaceMonitor *airspaceMonitor);

        protected:
            //! \copydoc QObject::event
            virtual bool event(QEvent *event) override;

        private:
            QScopedPointer<Ui::CInterpolationLogDisplayDialog> ui;
        };
    } // ns
} // ns

#endif // guard
