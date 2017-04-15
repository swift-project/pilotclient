/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKSIM_XBUS_MENUS_H
#define BLACKSIM_XBUS_MENUS_H

//! \file

#include <XPLM/XPLMMenus.h>
#include <functional>
#include <vector>
#include <memory>

namespace XBus
{

    class CMenu;

    /*!
     * Class-based interface to X-Plane SDK menu items.
     */
    class CMenuItem
    {
    public:
        //! Construct an uninitialized menu item object
        CMenuItem() = default;

        //! Returns true if the menu item is checked
        bool getChecked() const;

        //! Sets the checked status of the menu item
        void setChecked(bool checked);

        //! Enables or disabled the menu item
        void setEnabled(bool enabled);

    private:
        friend class CMenu;

        CMenuItem(XPLMMenuID parent, int item, bool checkable, bool checked);

        struct Data
        {
            Data(XPLMMenuID parent_, int item_, bool checkable_) : parent(parent_), item(item_), checkable(checkable_) {}
            XPLMMenuID parent;
            int item;
            bool checkable;
        };
        std::shared_ptr<Data> m_data;
    };

    /*!
     * Class-based interface to X-Plane SDK menus.
     */
    class CMenu
    {
    public:
        //! Construct an uninitialized menu object
        CMenu() = default;

        //! Returns a menu object representing the top-level menu of all plugins.
        static CMenu mainMenu();

        //! Appends an item to the menu and returns it.
        CMenuItem item(const std::string &name, std::function<void()> callback);

        //! Appends a checkbox item to the menu and returns it.
        CMenuItem checkableItem(const std::string &name, bool checked, std::function<void(bool)> callback);

        //! Appends a separator to the menu.
        void sep();

        //! Appends an item to the menu which opens a sub-menu, and returns it.
        CMenu subMenu(const std::string &name);

    private:
        typedef std::vector<std::pair<CMenuItem, std::function<void(bool)>>> ItemList;

        CMenu(XPLMMenuID id, bool isMainMenu, std::unique_ptr<ItemList> callbacks);

        static void handler(void *menuRef, void *itemRef);

        struct Data
        {
            Data(XPLMMenuID id_, bool isMainMenu_, std::unique_ptr<ItemList> items_)
                : id(id_), isMainMenu(isMainMenu_), items(std::move(items_)) {}
            XPLMMenuID id;
            bool isMainMenu;
            std::unique_ptr<ItemList> items;
            std::vector<CMenu> subMenus;
            ~Data();
            Data(const Data &) = delete;
            Data &operator =(const Data &) = delete;
        };
        std::shared_ptr<Data> m_data;
    };

}

#endif // guard
