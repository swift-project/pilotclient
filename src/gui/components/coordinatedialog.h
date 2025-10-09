// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_COORDINATEDIALOG_H
#define SWIFT_GUI_COMPONENTS_COORDINATEDIALOG_H

#include <QDialog>
#include <QScopedPointer>

#include "gui/editors/coordinateform.h"
#include "gui/swiftguiexport.h"

namespace Ui
{
    class CCoordinateDialog;
}
namespace swift::gui::components
{
    //! Coordinate form as dialog
    //! \sa swift::gui::editors::CCoordinateForm
    class SWIFT_GUI_EXPORT CCoordinateDialog : public QDialog
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CCoordinateDialog(QWidget *parent = nullptr);

        //! Destructor
        ~CCoordinateDialog() override;

        //! \copydoc swift::gui::editors::CCoordinateForm::getCoordinate
        swift::misc::geo::CCoordinateGeodetic getCoordinate() const;

        //! \copydoc swift::gui::editors::CCoordinateForm::setCoordinate
        void setCoordinate(const swift::misc::geo::ICoordinateGeodetic &coordinate);

        //! \copydoc swift::gui::editors::CCoordinateForm::setReadOnly
        void setReadOnly(bool readonly);

        //! \copydoc swift::gui::editors::CCoordinateForm::setSelectOnly
        void setSelectOnly();

        //! \copydoc swift::gui::editors::CCoordinateForm::showElevation
        void showElevation(bool show);

        //! \copydoc swift::gui::editors::CCoordinateForm::validate
        swift::misc::CStatusMessageList validate(bool nested = false) const;

    signals:
        //! \copydoc swift::gui::editors::CCoordinateForm::changedCoordinate
        void changedCoordinate();

    private:
        QScopedPointer<Ui::CCoordinateDialog> ui;

        //! disable default buttons
        void noDefaultButtons();
    };
} // namespace swift::gui::components

#endif // SWIFT_GUI_COMPONENTS_COORDINATEDIALOG_H
