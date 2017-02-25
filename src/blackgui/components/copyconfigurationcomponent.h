/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_COPYCONFIGURATION_H
#define BLACKGUI_COMPONENTS_COPYCONFIGURATION_H

#include "blackgui/blackguiexport.h"
#include <QFrame>
#include <QWizardPage>
#include <QDir>

namespace Ui { class CCopyConfigurationComponent; }
namespace BlackGui
{
    namespace Components
    {
        /**
         * Copy configuration (i.e. settings and cache files)
         */
        class BLACKGUI_EXPORT CCopyConfigurationComponent : public QFrame
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CCopyConfigurationComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CCopyConfigurationComponent();

            //! Cache mode
            void setCacheMode();

            //! Settings mode
            void setSettingsMode();

            //! Selected files are copied
            void copySelectedFiles();

            //! Preselect newer files
            void preselectMissingOurOutdated();

            //! Init file content
            void initCurrentDirectories();

        private:
            //! The current version changed
            void currentVersionChanged(const QString &text);

            //! This version's directory (cache or setting)
            const QString &getThisVersionDirectory() const;

            //! Get the selected directory
            QString getOtherVersionsSelectedDirectory() const;

            //! Get the selected files
            QStringList getSelectedFiles() const;

            //! Combobox width
            void setComboBoxWidth();

            QStringList m_versionDirs;
            QScopedPointer<Ui::CCopyConfigurationComponent> ui;
        };

        /**
         * Wizard page for CCopyConfigurationComponent
         */
        class CCopyConfigurationWizardPage : public QWizardPage
        {
        public:
            //! Constructors
            using QWizardPage::QWizardPage;

            //! Set config
            void setConfigComponent(CCopyConfigurationComponent *config) { m_config = config; }

            //! \copydoc QWizardPage::initializePage
            virtual void initializePage() override;

            //! \copydoc QWizardPage::validatePage
            virtual bool validatePage() override;

        private:
            CCopyConfigurationComponent *m_config = nullptr;
        };
    } // ns
} // ns

#endif // guard
