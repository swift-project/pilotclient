/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_DBLOADDATADIALOG_H
#define BLACKGUI_COMPONENTS_DBLOADDATADIALOG_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/network/entityflags.h"
#include "blackmisc/simulation/data/modelcaches.h"
#include <QDialog>
#include <QScopedPointer>
#include <QStringListModel>
#include <QAbstractButton>

namespace Ui { class CDbLoadDataDialog; }
namespace BlackGui
{
    namespace Components
    {
        /**
         * Load data from DB as dialog
         */
        class BLACKGUI_EXPORT CDbLoadDataDialog : public QDialog
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CDbLoadDataDialog(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CDbLoadDataDialog();

            //! Newer or empty entities detected
            bool newerOrEmptyEntitiesDetected(BlackMisc::Network::CEntityFlags::Entity loadEntities);

        private:
            //! The string list model
            QStringListModel *entitiesModel() const;

            //! All selected items
            QStringList selectedEntities() const;

            //! Button clicked
            void onButtonClicked(QAbstractButton *button);

            //! Data have been read
            void onDataRead(BlackMisc::Network::CEntityFlags::Entity entity, BlackMisc::Network::CEntityFlags::ReadState state, int number);

            //! Dialog rejected
            void onRejected();

            //! Consolidate
            void consolidate();

            QScopedPointer<Ui::CDbLoadDataDialog> ui;
            BlackMisc::Network::CEntityFlags::Entity m_pendingEntities = BlackMisc::Network::CEntityFlags::NoEntity;
            BlackMisc::Simulation::Data::CModelSetCaches m_sets { true, this }; //!< caches
            BlackMisc::Simulation::Data::CModelCaches m_models  { true, this }; //!< models
            int m_pendingEntitiesCount = -1;
            bool m_consolidating = false; //! currently consolidating
            bool m_autoConsolidate = false;
        };
    } // ns
} // ns

#endif // guard
