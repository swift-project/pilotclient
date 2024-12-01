// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_ALTITUDEDIALOG_H
#define SWIFT_GUI_COMPONENTS_ALTITUDEDIALOG_H

#include <QDialog>
#include <QScopedPointer>

#include "misc/aviation/altitude.h"

namespace Ui
{
    class CAltitudeDialog;
}
namespace swift::gui::components
{
    /*!
     * Dialog to get a correct altitude
     */
    class CAltitudeDialog : public QDialog
    {
        Q_OBJECT

    public:
        //! Which mode used to enter
        enum Mode
        {
            Unknown,
            StringOnly,
            VFR,
            FlightFlevelInFeet,
            MetricLevelInTensOfMeters,
            AltitudeInHundredsOfFeet,
            AltitudeInTensOfMeters,
            AltitudeInFeet,
            AltitudeInMeters,
            AltitudeInMetersConvertedToFeet
        };

        //! Constructor
        explicit CAltitudeDialog(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CAltitudeDialog() override;

        //! Get mode
        Mode getMode() const;

        //! Set VATSIM flag
        void setVatsim(bool vatsim);

        //! Manual string mode
        bool isStringOnly() const;

        //! Altitude string
        const swift::misc::aviation::CAltitude &getAltitude() const { return m_altitude; }

        //! Altitude string
        const QString &getAltitudeString() const { return m_altitudeStr; }

    private:
        QScopedPointer<Ui::CAltitudeDialog> ui;
        swift::misc::aviation::CAltitude m_altitude;
        QString m_altitudeStr;

        //! Edit finished
        void onEditFinished();

        //! VFR selected
        void onVFRSelected(bool selected);

        //! String only selected
        void onStringOnlySelected(bool selected);

        //! On text edit
        void onTextEdit(const QString &text);

        //! Simplified format changed
        void onSimplifiedVATSIMFormatChanged(bool checked);

        //! Update style sheet
        void updateStyleSheet();
    };
} // namespace swift::gui::components

#endif // SWIFT_GUI_COMPONENTS_ALTITUDEDIALOG_H
