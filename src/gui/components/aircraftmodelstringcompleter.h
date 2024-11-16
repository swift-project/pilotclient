// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPLETER_AIRCRAFTMODELSTRINGCOMPLETER_H
#define SWIFT_GUI_COMPLETER_AIRCRAFTMODELSTRINGCOMPLETER_H

#include <QCompleter>
#include <QFlags>
#include <QFrame>
#include <QScopedPointer>

#include "gui/swiftguiexport.h"
#include "misc/simulation/aircraftmodel.h"
#include "misc/simulation/data/modelcaches.h"

namespace Ui
{
    class CAircraftModelStringCompleter;
}
namespace swift::gui::components
{
    /*!
     * Completer for model strings
     */
    class SWIFT_GUI_EXPORT CAircraftModelStringCompleter : public QFrame
    {
        Q_OBJECT

    public:
        //! Sources for string completion
        enum CompleterSourceFlag
        {
            None = 0, //!< normally used to indicate nothing was loaded
            DB = 1 << 0,
            ModelSet = 1 << 1,
            OwnModels = 1 << 2
        };
        Q_DECLARE_FLAGS(CompleterSource, CompleterSourceFlag)

        //! Constructor
        explicit CAircraftModelStringCompleter(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CAircraftModelStringCompleter() override;

        //! The model string
        QString getModelString() const;

        //! Show the selection buttons
        void showSourceSelection(bool show);

        //! Set text
        void setText(const QString &completersString);

        //! Set model
        void setModel(const swift::misc::simulation::CAircraftModel &model);

        //! Show/hide radio buttons
        void setSourceVisible(CompleterSource source, bool visible);

        //! Set the currently selected source
        void selectSource(CompleterSourceFlag source);

        //! Change the simulator
        bool setSimulator(const swift::misc::simulation::CSimulatorInfo &simulator);

        //! Get current simulator
        swift::misc::simulation::CSimulatorInfo getSimulator() const;

        //! Clear
        void clear();

    signals:
        //! Model has been changed
        void modelStringChanged();

    private:
        //! Set the completer
        void setCompleter(bool simChanged);

        //! Init the GUI
        void initGui();

        //! Value has been changed
        void onTextChanged();

        //! Simulator connected
        void onSimulatorConnected(int status);

        //! All swift data have been read
        void onSwiftModelDataRead();

    private:
        QScopedPointer<Ui::CAircraftModelStringCompleter> ui;
        // model set completer data are fetched from context
        swift::misc::simulation::data::CModelCaches m_modelCaches { false, this }; //!< all models, works locally only
        swift::misc::simulation::CSimulatorInfo m_currentSimulator;
        CompleterSourceFlag m_currentDataSource = None;
    };
} // namespace swift::gui::components

#endif // guard
