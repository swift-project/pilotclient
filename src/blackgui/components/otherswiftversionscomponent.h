/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_OTHERSWIFTVERSIONS_H
#define BLACKGUI_COMPONENTS_OTHERSWIFTVERSIONS_H

#include "blackmisc/applicationinfo.h"
#include <QFrame>
#include <QScopedPointer>

namespace Ui { class COtherSwiftVersionsComponent; }
namespace BlackGui::Components
{
    /*!
     * Overview about other "swift" versions
     */
    class COtherSwiftVersionsComponent : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit COtherSwiftVersionsComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~COtherSwiftVersionsComponent();

        //! Any version selected?
        bool hasSelection() const;

        //! Get the selected other version
        BlackMisc::CApplicationInfo selectedOtherVersion() const;

        //! Reload other versions
        void reloadOtherVersions() { this->reloadOtherVersionsDeferred(0); }

        //! Reload versions
        void reloadOtherVersionsDeferred(int deferMs);

    signals:
        //! Selection changed
        void versionChanged(const BlackMisc::CApplicationInfo &info);

    private:
        QScopedPointer<Ui::COtherSwiftVersionsComponent> ui;

        //! Data directory
        void openDataDirectory();

        //! Object has been selected
        void onObjectSelected(const BlackMisc::CVariant &object);
    };
} // ns

#endif // guard
