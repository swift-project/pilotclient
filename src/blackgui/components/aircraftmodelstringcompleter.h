/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPLETER_AIRCRAFTMODELSTRINGCOMPLETER_H
#define BLACKGUI_COMPLETER_AIRCRAFTMODELSTRINGCOMPLETER_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/simulation/data/modelcaches.h"
#include "blackmisc/simulation/aircraftmodel.h"

#include <QFrame>
#include <QScopedPointer>
#include <QCompleter>
#include <QFlags>

namespace Ui { class CAircraftModelStringCompleter; }
namespace BlackGui::Components
{
    /*!
     * Completer for model strings
     */
    class BLACKGUI_EXPORT CAircraftModelStringCompleter : public QFrame
    {
        Q_OBJECT

    public:

        //! Sources for string completion
        enum CompleterSourceFlag
        {
            None        = 0,      //!< normally used to indicate nothing was loaded
            DB          = 1 << 0,
            ModelSet    = 1 << 1,
            OwnModels   = 1 << 2
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
        void setModel(const BlackMisc::Simulation::CAircraftModel &model);

        //! Show/hide radio buttons
        void setSourceVisible(CompleterSource source, bool visible);

        //! Set the currently selected source
        void selectSource(CompleterSourceFlag source);

        //! Change the simulator
        bool setSimulator(const BlackMisc::Simulation::CSimulatorInfo &simulator);

        //! Get current simulator
        BlackMisc::Simulation::CSimulatorInfo getSimulator() const;

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
        QScopedPointer <Ui::CAircraftModelStringCompleter> ui;
        // model set completer data are fetched from context
        BlackMisc::Simulation::Data::CModelCaches m_modelCaches { false, this }; //!< all models, works locally only
        BlackMisc::Simulation::CSimulatorInfo m_currentSimulator;
        CompleterSourceFlag m_currentDataSource = None;
    };
} // ns

#endif // guard
