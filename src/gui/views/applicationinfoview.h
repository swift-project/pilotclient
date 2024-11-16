// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_VIEWS_APPLICATIONINFOVIEW_H
#define SWIFT_GUI_VIEWS_APPLICATIONINFOVIEW_H

#include <QAction>

#include "gui/menus/menudelegate.h"
#include "gui/models/applicationinfolistmodel.h"
#include "gui/swiftguiexport.h"
#include "gui/views/viewbase.h"

namespace swift::gui::views
{
    //! Network servers
    class SWIFT_GUI_EXPORT CApplicationInfoView : public CViewBase<models::CApplicationInfoListModel>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CApplicationInfoView(QWidget *parent = nullptr);

        //! swift::misc::CApplicationInfoList::otherSwiftVersionsFromDataDirectories
        int otherSwiftVersionsFromDataDirectories(bool reinitOtherVersions = false);

        //! Display versions and resize
        int otherSwiftVersionsFromDataDiretoriesAndResize(bool reinitOtherVersion = false);

        //! Delete the selected directories
        void deleteSelectedDataDirectories();
    };

    //! Menu base class for aircraft model view menus
    class CApplicationInfoMenu : public menus::IMenuDelegate
    {
        Q_OBJECT

    public:
        //! Constructor
        CApplicationInfoMenu(CApplicationInfoView *modelView) : menus::IMenuDelegate(modelView) {}

        //! \copydoc menus::IMenuDelegate::customMenu
        virtual void customMenu(menus::CMenuActions &menuActions) override;

    private:
        //! Model view
        CApplicationInfoView *view() const;

        QAction *m_menuActionDeleteDirectory = nullptr; //!< action to delete menu
    };
} // namespace swift::gui::views
#endif // guard
