/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_DBOWNMODELSETDIALOG_H
#define BLACKGUI_COMPONENTS_DBOWNMODELSETDIALOG_H

#include "blackgui/components/dbmappingcomponentaware.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/simulatorinfo.h"

#include <QDialog>
#include <QObject>
#include <QScopedPointer>

class QWidget;

namespace BlackMisc { class CLogCategoryList; }
namespace Ui { class CDbOwnModelSetDialog; }

namespace BlackGui
{
    namespace Components
    {
        /*!
         * Dialog to create new model set
         */
        class CDbOwnModelSetDialog :
            public QDialog,
            public CDbMappingComponentAware
        {
            Q_OBJECT

        public:
            //! Categories
            static const BlackMisc::CLogCategoryList &getLogCategories();

            //! Constructor
            explicit CDbOwnModelSetDialog(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CDbOwnModelSetDialog();

            //! Last build set
            const BlackMisc::Simulation::CAircraftModelList &getModelSet() const { return m_modelSet; }

            //! Simulator info
            const BlackMisc::Simulation::CSimulatorInfo &getSimulatorInfo() const { return m_simulatorInfo; }

            //! Reload data
            void reloadData();

            //! Exec and display simulator
            virtual int exec() override;

        private slots:
            //! Button clicked
            void ps_buttonClicked();

        private:
            QScopedPointer<Ui::CDbOwnModelSetDialog>  ui;
            BlackMisc::Simulation::CAircraftModelList m_modelSet;
            BlackMisc::Simulation::CSimulatorInfo     m_simulatorInfo;

            //! Build the set
            BlackMisc::Simulation::CAircraftModelList buildSet(const BlackMisc::Simulation::CSimulatorInfo &simulator, const BlackMisc::Simulation::CAircraftModelList &currentSet = {});
        };
    } // ns
} // ns

#endif // guard
