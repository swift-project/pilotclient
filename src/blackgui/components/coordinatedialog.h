// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_COORDINATEDIALOG_H
#define BLACKGUI_COMPONENTS_COORDINATEDIALOG_H

#include "blackgui/blackguiexport.h"
#include "blackgui/editors/coordinateform.h"

#include <QDialog>
#include <QScopedPointer>

namespace Ui
{
    class CCoordinateDialog;
}
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
        swift::misc::geo::CCoordinateGeodetic getCoordinate() const;

        //! \copydoc BlackGui::Editors::CCoordinateForm::setCoordinate
        void setCoordinate(const swift::misc::geo::ICoordinateGeodetic &coordinate);

        //! \copydoc BlackGui::Editors::CCoordinateForm::setReadOnly
        void setReadOnly(bool readonly);

        //! \copydoc BlackGui::Editors::CCoordinateForm::setSelectOnly
        void setSelectOnly();

        //! \copydoc BlackGui::Editors::CCoordinateForm::showElevation
        void showElevation(bool show);

        //! \copydoc BlackGui::Editors::CCoordinateForm::validate
        swift::misc::CStatusMessageList validate(bool nested = false) const;

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
