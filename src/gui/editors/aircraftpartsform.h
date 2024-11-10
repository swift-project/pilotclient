// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_EDITORS_AIRCRAFTPARTSFORM_H
#define SWIFT_GUI_EDITORS_AIRCRAFTPARTSFORM_H

#include "gui/editors/form.h"
#include "gui/swiftguiexport.h"
#include "misc/aviation/aircraftparts.h"
#include "misc/statusmessagelist.h"

class QWidget;

namespace Ui
{
    class CAircraftPartsForm;
}
namespace swift::gui::editors
{
    /*!
     * Edit and set aircraft parts
     */
    class SWIFT_GUI_EXPORT CAircraftPartsForm : public CForm
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAircraftPartsForm(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CAircraftPartsForm() override;

        //! \name Form class implementations
        //! @{
        virtual void setReadOnly(bool readonly) override;
        virtual void setSelectOnly() override;
        virtual swift::misc::CStatusMessageList validate(bool nested = false) const override;
        //! @}

        //! Get the parts
        swift::misc::aviation::CAircraftParts getAircraftPartsFromGui() const;

        //! Get the parts from JSON
        swift::misc::aviation::CAircraftParts getAircraftPartsFromJson() const;

        //! Set the parts
        void setAircraftParts(const swift::misc::aviation::CAircraftParts &parts);

        //! Set button visible
        void showSetButton(bool visible);

        //! Set JSON textarea visible
        void showJson(bool visible);

    signals:
        //! Change coordinate
        void changeAircraftParts();

    private:
        QScopedPointer<Ui::CAircraftPartsForm> ui;

        //! Get parts object
        swift::misc::aviation::CAircraftParts guiToAircraftParts() const;

        //! GUI set by parts
        void partsToGui(const swift::misc::aviation::CAircraftParts &parts);

        //! GUI values to JSON
        void guiToJson();

        //! All lights on
        void setAllLights();

        //! All engines on
        void setAllEngines();
    };
} // ns
#endif // guard
