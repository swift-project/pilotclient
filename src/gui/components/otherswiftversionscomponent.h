// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_OTHERSWIFTVERSIONS_H
#define SWIFT_GUI_COMPONENTS_OTHERSWIFTVERSIONS_H

#include <QFrame>
#include <QScopedPointer>

#include "misc/applicationinfo.h"

namespace Ui
{
    class COtherSwiftVersionsComponent;
}
namespace swift::gui::components
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
        swift::misc::CApplicationInfo selectedOtherVersion() const;

        //! Reload other versions
        void reloadOtherVersions() { this->reloadOtherVersionsDeferred(0); }

        //! Reload versions
        void reloadOtherVersionsDeferred(int deferMs);

    signals:
        //! Selection changed
        void versionChanged(const swift::misc::CApplicationInfo &info);

    private:
        QScopedPointer<Ui::COtherSwiftVersionsComponent> ui;

        //! Data directory
        void openDataDirectory();

        //! Object has been selected
        void onObjectSelected(const swift::misc::CVariant &object);
    };
} // namespace swift::gui::components

#endif // SWIFT_GUI_COMPONENTS_OTHERSWIFTVERSIONS_H
