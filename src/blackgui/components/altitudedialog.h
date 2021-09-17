/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_ALTITUDEDIALOG_H
#define BLACKGUI_COMPONENTS_ALTITUDEDIALOG_H

#include "blackmisc/aviation/altitude.h"

#include <QDialog>
#include <QScopedPointer>

namespace Ui { class CAltitudeDialog; }
namespace BlackGui::Components
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
        const BlackMisc::Aviation::CAltitude &getAltitude() const { return m_altitude; }

        //! Altitude string
        const QString &getAltitudeString() const { return m_altitudeStr; }

    private:
        QScopedPointer<Ui::CAltitudeDialog> ui;
        BlackMisc::Aviation::CAltitude m_altitude;
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
} // ns

#endif // guard
