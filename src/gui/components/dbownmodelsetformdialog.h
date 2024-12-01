// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_DBOWNMODELSETFORMDIALOG_H
#define SWIFT_GUI_COMPONENTS_DBOWNMODELSETFORMDIALOG_H

#include <QDialog>
#include <QObject>
#include <QScopedPointer>

#include "gui/components/dbmappingcomponentaware.h"
#include "misc/simulation/aircraftmodellist.h"
#include "misc/simulation/simulatorinfo.h"

namespace Ui
{
    class CDbOwnModelSetFormDialog;
}
namespace swift::misc
{
    class CLogCategoryList;
}
namespace swift::gui::components
{
    /*!
     * Dialog to create new model set
     */
    class CDbOwnModelSetFormDialog : public QDialog, public CDbMappingComponentAware
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
        const swift::misc::simulation::CAircraftModelList &getModelSet() const { return m_modelSet; }

        //! Init last set
        void setModelSet(const swift::misc::simulation::CAircraftModelList &models) { m_modelSet = models; }

        //! Simulator info
        const swift::misc::simulation::CSimulatorInfo &getSimulatorInfo() const { return m_simulatorInfo; }

        //! Reload data e.g. current model set and simulator
        void reloadData();

        //! Exec and display simulator
        virtual int exec() override;

    protected:
        //! \copydoc QObject::event
        virtual bool event(QEvent *event) override;

    private:
        QScopedPointer<Ui::CDbOwnModelSetFormDialog> ui;
        swift::misc::simulation::CAircraftModelList m_modelSet;
        swift::misc::simulation::CSimulatorInfo m_simulatorInfo;

        //! Button clicked
        void buttonClicked();

        //! Simulator changed
        void simulatorChanged(const swift::misc::simulation::CSimulatorInfo &simulator);

        //! Check data
        bool checkData();

        //! Set current simulator
        void setSimulator(const swift::misc::simulation::CSimulatorInfo &simulator);

        //! Build the set
        swift::misc::simulation::CAircraftModelList
        buildSet(const swift::misc::simulation::CSimulatorInfo &simulator,
                 const swift::misc::simulation::CAircraftModelList &currentSet = {});
    };
} // namespace swift::gui::components

#endif // SWIFT_GUI_COMPONENTS_DBOWNMODELSETFORMDIALOG_H
