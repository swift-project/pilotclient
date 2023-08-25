// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_VIEWS_APPLICATIONINFOVIEW_H
#define BLACKGUI_VIEWS_APPLICATIONINFOVIEW_H

#include "blackgui/views/viewbase.h"
#include "blackgui/models/applicationinfolistmodel.h"
#include "blackgui/menus/menudelegate.h"
#include "blackgui/blackguiexport.h"

#include <QAction>

namespace BlackGui::Views
{
    //! Network servers
    class BLACKGUI_EXPORT CApplicationInfoView : public CViewBase<Models::CApplicationInfoListModel>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CApplicationInfoView(QWidget *parent = nullptr);

        //! BlackMisc::CApplicationInfoList::otherSwiftVersionsFromDataDirectories
        int otherSwiftVersionsFromDataDirectories(bool reinitOtherVersions = false);

        //! Display versions and resize
        int otherSwiftVersionsFromDataDiretoriesAndResize(bool reinitOtherVersion = false);

        //! Delete the selected directories
        void deleteSelectedDataDirectories();
    };

    //! Menu base class for aircraft model view menus
    class CApplicationInfoMenu : public Menus::IMenuDelegate
    {
        Q_OBJECT

    public:
        //! Constructor
        CApplicationInfoMenu(CApplicationInfoView *modelView) : Menus::IMenuDelegate(modelView)
        {}

        //! \copydoc Menus::IMenuDelegate::customMenu
        virtual void customMenu(Menus::CMenuActions &menuActions) override;

    private:
        //! Model view
        CApplicationInfoView *view() const;

        QAction *m_menuActionDeleteDirectory = nullptr; //!< action to delete menu
    };
} // ns
#endif // guard
