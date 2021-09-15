/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_DBAIRLINENAMESELECTORCOMPONENT_H
#define BLACKGUI_COMPONENTS_DBAIRLINENAMESELECTORCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "blackgui/components/dbairlineicaoselectorbase.h"
#include "blackmisc/aviation/airlineicaocode.h"

#include <QObject>
#include <QScopedPointer>

class QCompleter;
class QWidget;

namespace Ui { class CDbAirlineNameSelectorComponent; }
namespace BlackGui::Components
{
    /*!
     * Select airline by name
     */
    class BLACKGUI_EXPORT CDbAirlineNameSelectorComponent : public CDbAirlineIcaoSelectorBase
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CDbAirlineNameSelectorComponent(QWidget *parent = nullptr);

        //! Destructor
        //! \note needed for forward declared QScopedPointer and needs to be in .cpp
        virtual ~CDbAirlineNameSelectorComponent() override;

        //! \name Base class overrides
        //! @{
        virtual void setReadOnly(bool readOnly) override;
        virtual bool setAirlineIcao(const BlackMisc::Aviation::CAirlineIcaoCode &icao) override;
        virtual void clear() override;
        //! @}

    protected:
        //! \copydoc CDbAirlineIcaoSelectorBase::createCompleter
        virtual QCompleter *createCompleter() override;

    private:
        QScopedPointer<Ui::CDbAirlineNameSelectorComponent> ui;

    private slots:
        //! Data changed
        void ps_dataChanged();
    };
} // ns

#endif // guard
