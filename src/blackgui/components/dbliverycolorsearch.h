/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_DBLIVERYCOLORSEARCH_H
#define BLACKGUI_COMPONENTS_DBLIVERYCOLORSEARCH_H

#include "blackmisc/aviation/livery.h"
#include <QDialog>
#include <QScopedPointer>

namespace Ui { class CDbLiveryColorSearch; }
namespace BlackGui
{
    namespace Components
    {
        /*!
         * Search for best color livery
         */
        class CDbLiveryColorSearch : public QDialog
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CDbLiveryColorSearch(QWidget *parent = nullptr);

            //! Destructor
            ~CDbLiveryColorSearch();

            //! Found livery if any, otherwise default
            BlackMisc::Aviation::CLivery getLivery() const;

        private:
            //! Dialog has been accepted
            void ps_onAccepted();

            BlackMisc::Aviation::CLivery m_foundLivery; //!< lat livery found

        private:
            QScopedPointer<Ui::CDbLiveryColorSearch> ui;
        };
    } // ns
} // ns

#endif // guard
