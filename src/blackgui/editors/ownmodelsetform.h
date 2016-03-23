/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_EDITORS_OWNMODELSETFORM_H
#define BLACKGUI_EDITORS_OWNMODELSETFORM_H

#include "blackmisc/simulation/distributorlist.h"
#include <QFrame>
#include <QScopedPointer>

namespace Ui { class COwnModelSetForm; }

namespace BlackGui
{
    namespace Editors
    {
        /*!
         * Selection for own model set
         */
        class COwnModelSetForm : public QFrame
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit COwnModelSetForm(QWidget *parent = nullptr);

            //! Destructor
            ~COwnModelSetForm();

            //! Reload data
            void reloadData();

            //! Selected providers?
            bool selectedDistributors() const;

            //! Get selected providers
            BlackMisc::Simulation::CDistributorList getSelectedDistributors() const;

            //! Only DB data
            bool dbDataOnly() const;

            //! DB ICAO codes
            bool dbIcaoCodesOnly() const;

            //! Request incremental build
            bool incrementalBuild() const;

        private:
            QScopedPointer<Ui::COwnModelSetForm> ui;
        };
    } // ns
} // ns

#endif // guard
