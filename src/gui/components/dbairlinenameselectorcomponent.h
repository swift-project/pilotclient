// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_DBAIRLINENAMESELECTORCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_DBAIRLINENAMESELECTORCOMPONENT_H

#include <QObject>
#include <QScopedPointer>

#include "gui/components/dbairlineicaoselectorbase.h"
#include "gui/swiftguiexport.h"
#include "misc/aviation/airlineicaocode.h"

class QCompleter;
class QWidget;

namespace Ui
{
    class CDbAirlineNameSelectorComponent;
}
namespace swift::gui::components
{
    /*!
     * Select airline by name
     */
    class SWIFT_GUI_EXPORT CDbAirlineNameSelectorComponent : public CDbAirlineIcaoSelectorBase
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CDbAirlineNameSelectorComponent(QWidget *parent = nullptr);

        //! Destructor
        //! \note needed for forward declared QScopedPointer and needs to be in .cpp
        ~CDbAirlineNameSelectorComponent() override;

        //! \name Base class overrides
        //! @{
        void setReadOnly(bool readOnly) override;
        bool setAirlineIcao(const swift::misc::aviation::CAirlineIcaoCode &icao) override;
        void clear() override;
        //! @}

    protected:
        //! \copydoc CDbAirlineIcaoSelectorBase::createCompleter
        QCompleter *createCompleter() override;

    private:
        QScopedPointer<Ui::CDbAirlineNameSelectorComponent> ui;

    private slots:
        //! Data changed
        void ps_dataChanged();
    };
} // namespace swift::gui::components

#endif // SWIFT_GUI_COMPONENTS_DBAIRLINENAMESELECTORCOMPONENT_H
