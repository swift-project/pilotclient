/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_DBOWNMODELSETFORMDIALOG_H
#define BLACKGUI_COMPONENTS_DBOWNMODELSETFORMDIALOG_H

#include "blackgui/components/dbmappingcomponentaware.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/simulatorinfo.h"

#include <QDialog>
#include <QObject>
#include <QScopedPointer>

namespace Ui { class CDbOwnModelSetFormDialog; }
namespace BlackMisc { class CLogCategoryList; }
namespace BlackGui::Components
{
    /*!
     * Dialog to create new model set
     */
    class CDbOwnModelSetFormDialog :
        public QDialog,
        public CDbMappingComponentAware
    {
        Q_OBJECT

    public:
        //! Categories
        static const QStringList &getLogCategories();

        //! Constructor
        explicit CDbOwnModelSetFormDialog(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CDbOwnModelSetFormDialog() override;

        //! Last build set
        const BlackMisc::Simulation::CAircraftModelList &getModelSet() const { return m_modelSet; }

        //! Init last set
        void setModelSet(const BlackMisc::Simulation::CAircraftModelList &models) { m_modelSet = models; }

        //! Simulator info
        const BlackMisc::Simulation::CSimulatorInfo &getSimulatorInfo() const { return m_simulatorInfo; }

        //! Reload data e.g. current model set and simulator
        void reloadData();

        //! Exec and display simulator
        virtual int exec() override;

    protected:
        //! \copydoc QObject::event
        virtual bool event(QEvent *event) override;

    private:
        QScopedPointer<Ui::CDbOwnModelSetFormDialog> ui;
        BlackMisc::Simulation::CAircraftModelList m_modelSet;
        BlackMisc::Simulation::CSimulatorInfo     m_simulatorInfo;

        //! Button clicked
        void buttonClicked();

        //! Simulator changed
        void simulatorChanged(const BlackMisc::Simulation::CSimulatorInfo &simulator);

        //! Check data
        bool checkData();

        //! Set current simulator
        void setSimulator(const BlackMisc::Simulation::CSimulatorInfo &simulator);

        //! Build the set
        BlackMisc::Simulation::CAircraftModelList buildSet(const BlackMisc::Simulation::CSimulatorInfo &simulator, const BlackMisc::Simulation::CAircraftModelList &currentSet = {});
    };
} // ns

#endif // guard
