/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_SIMULATORCOMPONENT_H
#define BLACKGUI_SIMULATORCOMPONENT_H

#include "runtimebasedcomponent.h"
#include "blackmisc/icon.h"
#include <QTabWidget>

namespace Ui { class CSimulatorComponent; }
namespace BlackGui
{
    namespace Components
    {

        //! Simulator component
        class CSimulatorComponent : public QTabWidget, public CRuntimeBasedComponent
        {
            Q_OBJECT

        public:

            //! Constructor
            explicit CSimulatorComponent(QWidget *parent = nullptr);

            //! Destructor
            ~CSimulatorComponent();

            //! Simple add or update name / value pair
            void addOrUpdateByName(const QString &name, const QString &value, const BlackMisc::CIcon &icon);

            //! Simple add or update name / value pair
            void addOrUpdateByName(const QString &name, const QString &value, BlackMisc::CIcons::IconIndex iconIndex);

            //! Number of entries
            int rowCount() const;

            //! Clear
            void clear();

        private:
            Ui::CSimulatorComponent *ui;
        };
    }
}

#endif // guard
