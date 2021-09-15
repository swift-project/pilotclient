/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_COORDINATEDIALOG_H
#define BLACKGUI_COMPONENTS_COORDINATEDIALOG_H

#include "blackgui/blackguiexport.h"
#include "blackgui/editors/coordinateform.h"

#include <QDialog>
#include <QScopedPointer>

namespace Ui { class CCoordinateDialog; }
namespace BlackGui::Components
{
    //! Coordinate form as dialog
    //! \sa BlackGui::Editors::CCoordinateForm
    class BLACKGUI_EXPORT CCoordinateDialog : public QDialog
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CCoordinateDialog(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CCoordinateDialog();

        //! \copydoc BlackGui::Editors::CCoordinateForm::getCoordinate
        BlackMisc::Geo::CCoordinateGeodetic getCoordinate() const;

        //! \copydoc BlackGui::Editors::CCoordinateForm::setCoordinate
        void setCoordinate(const BlackMisc::Geo::ICoordinateGeodetic &coordinate);

        //! \copydoc BlackGui::Editors::CCoordinateForm::setReadOnly
        void setReadOnly(bool readonly);

        //! \copydoc BlackGui::Editors::CCoordinateForm::setSelectOnly
        void setSelectOnly();

        //! \copydoc BlackGui::Editors::CCoordinateForm::showElevation
        void showElevation(bool show);

        //! \copydoc BlackGui::Editors::CCoordinateForm::validate
        BlackMisc::CStatusMessageList validate(bool nested = false) const;

    signals:
        //! \copydoc BlackGui::Editors::CCoordinateForm::changedCoordinate
        void changedCoordinate();

    private:
        QScopedPointer<Ui::CCoordinateDialog> ui;

        //! disable default buttons
        void noDefaultButtons();
    };
} // ns

#endif // guard
