// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef SWIFT_SIM_XSWIFTBUS_MENUS_H
#define SWIFT_SIM_XSWIFTBUS_MENUS_H

//! \file

#include <XPLM/XPLMMenus.h>

#include <functional>
#include <list>
#include <memory>
#include <string>

namespace XSwiftBus
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

        CMenuItem(XPLMMenuID parent, int item, bool checkable, std::function<void(bool)> callback);

        void setIndex(int index) { m_data->index = index; }

        struct Data
        {
            Data(XPLMMenuID parent_, int index_, bool checkable_, std::function<void(bool)> callback_)
                : parent(parent_), index(index_), checkable(checkable_), callback(callback_)
            {}
            XPLMMenuID parent;
            int index;
            bool checkable;
            std::function<void(bool)> callback;
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

        //! Removes item from the menu
        void removeItem(const CMenuItem &item);

        //! Appends a separator to the menu.
        void sep();

        //! Appends an item to the menu which opens a sub-menu, and returns it.
        CMenu subMenu(const std::string &name);

    private:
        // Using std::list, since it does not invalidate pointers.
        using ItemList = std::list<CMenuItem>;

        // CMenu(XPLMMenuID id, bool isMainMenu, std::unique_ptr<ItemList> callbacks);
        CMenu(XPLMMenuID id, bool isMainMenu, std::unique_ptr<ItemList> items);

        static void handler(void *menuRef, void *itemRef);

        struct Data
        {
            Data(XPLMMenuID id_, bool isMainMenu_, std::unique_ptr<ItemList> items_)
                : id(id_), isMainMenu(isMainMenu_), items(std::move(items_))
            {}
            XPLMMenuID id;
            bool isMainMenu;
            std::unique_ptr<ItemList> items;
            ~Data();
            Data(const Data &) = delete;
            Data &operator=(const Data &) = delete;
        };
        std::shared_ptr<Data> m_data;
    };

} // namespace XSwiftBus

#endif // guard
