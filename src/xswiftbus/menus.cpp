/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \cond PRIVATE

#include "menus.h"
#include <type_traits>
#include <cassert>
#include <string>
#include <algorithm>

namespace XSwiftBus
{
    //! "safe" cast from integer to void*
    template <typename T> void *voidptr_cast(T i)
    {
        static_assert(std::is_integral_v<T>, "voidptr_cast expects an integer");
        using intptr_type = std::conditional_t<std::is_signed_v<T>, intptr_t, uintptr_t>;
        return reinterpret_cast<void *>(static_cast<intptr_type>(i));
    }

    //! "safe" cast from void* to integer
    template <typename T> T intptr_cast(void *p)
    {
        static_assert(std::is_integral<T>::value, "voidptr_cast returns an integer");
        using intptr_type = std::conditional_t<std::is_signed_v<T>, intptr_t, uintptr_t>;
        return static_cast<T>(reinterpret_cast<intptr_type>(p));
    }

    CMenu::CMenu(XPLMMenuID id, bool isMainMenu, std::unique_ptr<ItemList> items)
        : m_data(std::make_shared<Data>(id, isMainMenu, std::move(items)))
    {}

    CMenu::Data::~Data()
    {
        if (! isMainMenu)
        {
            XPLMDestroyMenu(id);
        }
    }

    CMenu CMenu::mainMenu()
    {
        return { XPLMFindPluginsMenu(), true, nullptr };
    }

    CMenuItem CMenu::item(const std::string &name, std::function<void()> callback)
    {
        assert(! name.empty());
        m_data->items->emplace_back(
            CMenuItem { m_data->id, 0, false, [callback](bool) { callback(); } }
        );
        auto &menuItem = m_data->items->back();
        menuItem.setIndex(XPLMAppendMenuItem(m_data->id, name.c_str(), &menuItem, false));
        return menuItem;
    }

    CMenuItem CMenu::checkableItem(const std::string &name, bool checked, std::function<void(bool)> callback)
    {
        assert(!name.empty());
        m_data->items->emplace_back(
            CMenuItem{ m_data->id, 0, true, callback }
        );
        auto &menuItem = m_data->items->back();
        menuItem.setIndex(XPLMAppendMenuItem(m_data->id, name.c_str(), &menuItem, false));
        menuItem.setChecked(checked);
        return menuItem;
    }

    void CMenu::removeItem(const CMenuItem &item)
    {
        auto it = std::find_if(m_data->items->begin(), m_data->items->end(), [ = ](const auto & i)
        {
            return i.m_data->index == item.m_data->index;
        });

        XPLMRemoveMenuItem(m_data->id, it->m_data->index);
        it = m_data->items->erase(it);

        // Decrement the index of all below menu items
        while (it != m_data->items->end())
        {
            it->m_data->index--;
            ++it;
        }
    }

    void CMenu::sep()
    {
        XPLMAppendMenuSeparator(m_data->id);
    }

    CMenu CMenu::subMenu(const std::string &name)
    {
        assert(! name.empty());
        // auto items = std::make_unique<ItemList>();
        auto items = std::make_unique<ItemList>();
        auto itemsVoidPtr = static_cast<void *>(&*items);
        return { XPLMCreateMenu(name.c_str(), m_data->id, XPLMAppendMenuItem(m_data->id, name.c_str(), nullptr, false), handler, itemsVoidPtr), false, std::move(items) };
    }

    void CMenu::handler(void *menuRef, void *itemRef)
    {
        if (menuRef && itemRef)
        {
            CMenuItem *menuItem = static_cast<CMenuItem *>(itemRef);
            menuItem->m_data->callback(menuItem->getChecked());
        }
    }

    CMenuItem::CMenuItem(XPLMMenuID parent, int item, bool checkable, std::function<void(bool)> callback)
        : m_data(std::make_shared<Data>(parent, item, checkable, callback))
    {
    }

    bool CMenuItem::getChecked() const
    {
        XPLMMenuCheck check = xplm_Menu_NoCheck;
        XPLMCheckMenuItemState(m_data->parent, m_data->index, &check);
        return check == xplm_Menu_Checked;
    }

    void CMenuItem::setChecked(bool checked)
    {
        XPLMCheckMenuItem(m_data->parent, m_data->index, checked ? xplm_Menu_Checked : xplm_Menu_Unchecked);
    }

    void CMenuItem::setEnabled(bool enabled)
    {
        XPLMEnableMenuItem(m_data->parent, m_data->index, enabled);
    }

}

//! \endcond
