// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_AIRCRAFTCOMBINEDTYPESELECTOR_H
#define SWIFT_GUI_AIRCRAFTCOMBINEDTYPESELECTOR_H

#include <QFrame>
#include <QObject>
#include <QScopedPointer>
#include <QString>

#include "gui/swiftguiexport.h"
#include "misc/aviation/aircrafticaocode.h"

namespace Ui
{
    class CAircraftCombinedTypeSelector;
}
namespace swift::misc::aviation
{
    class CAircraftIcaoCode;
}
namespace swift::gui::components
{
    /*!
     * Select by comined type ("L2J", "H1T", ...)
     */
    class SWIFT_GUI_EXPORT CAircraftCombinedTypeSelector : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAircraftCombinedTypeSelector(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CAircraftCombinedTypeSelector() override;

        //! Set comined code, e.g. L1P
        void setCombinedType(const QString &combinedCode);

        //! Combined code from aircraft ICAO
        void setCombinedType(const swift::misc::aviation::CAircraftIcaoCode &icao);

        //! Clear
        void clear();

        //! Read only
        void setReadOnly(bool readOnly);

        //! Get the combined type, e.g. "L2P"
        QString getCombinedType() const;

    signals:
        //! Combined type has beend changed
        void changedCombinedType(const QString &cominedType);

    private:
        //! Code has been entered
        void combinedTypeEntered();

        //! Changed combobox
        void changedComboBox(const QString &text);

        //! Combined type from comboboxes
        QString getCombinedTypeFromComboBoxes() const;

        QString m_cc;
        QScopedPointer<Ui::CAircraftCombinedTypeSelector> ui;
    };
} // namespace swift::gui::components

#endif // guard
