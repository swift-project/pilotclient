/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_AIRCRAFTCOMBINEDTYPESELECTOR_H
#define BLACKGUI_AIRCRAFTCOMBINEDTYPESELECTOR_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/aviation/aircrafticaocode.h"

#include <QFrame>
#include <QObject>
#include <QScopedPointer>
#include <QString>

namespace Ui { class CAircraftCombinedTypeSelector; }
namespace BlackMisc::Aviation { class CAircraftIcaoCode; }
namespace BlackGui::Components
{
    /*!
     * Select by comined type ("L2J", "H1T", ...)
     */
    class BLACKGUI_EXPORT CAircraftCombinedTypeSelector : public QFrame
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
        void setCombinedType(const BlackMisc::Aviation::CAircraftIcaoCode &icao);

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
} // ns

#endif // guard
