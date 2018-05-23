/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_CONFIGURATIONWIZARD_H
#define BLACKGUI_COMPONENTS_CONFIGURATIONWIZARD_H

#include "blackgui/blackguiexport.h"
#include <QWizard>
#include <QScopedPointer>

namespace Ui { class CConfigurationWizard; }
namespace BlackGui
{
    namespace Components
    {
        /**
         * Configure the most important settings
         */
        class BLACKGUI_EXPORT CConfigurationWizard : public QWizard
        {
            Q_OBJECT

        public:
            //! Page ids
            enum Pages
            {
                Legal,
                CopyModels,
                CopySettings,
                CopyCaches,
                ConfigSimulator,
                XSwiftBus,
                DataLoad,
                FirstModelSet,
                ConfigHotkeys
            };

            //! Constructor
            explicit CConfigurationWizard(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CConfigurationWizard();

            //! Was the last step skipped?
            bool lastStepSkipped() const;

            //! Static version of CConfigurationWizard::lastStepSkipped
            static bool lastWizardStepSkipped(const QWizard *standardWizard);

        protected:
            //! \copydoc QObject::event
            virtual bool event(QEvent *event) override;

        private:
            //! The current page has changed
            void wizardCurrentIdChanged(int id);

            //! Custom button was clicked
            void clickedCustomButton(int which);

            //! Accepted or rejected
            void ended();

            //! Set the parent's window opacity
            void setParentOpacity(qreal opacity);

            QScopedPointer<Ui::CConfigurationWizard> ui;
            int m_previousId = -1;
            int m_minId = -1;
            int m_maxId = -1;
            bool m_skipped = false;
        };
    } // ns
} // ns
#endif // guard
