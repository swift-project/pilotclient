/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \cond PRIVATE

#include "menus.h"
#include <type_traits>
#include <cassert>
#include <string>

namespace XBus
{
    //! "safe" cast from integer to void*
    template <typename T> void *voidptr_cast(T i)
    {
        static_assert(std::is_integral<T>::value, "voidptr_cast expects an integer");
        using intptr_type = std::conditional_t<std::is_signed<T>::value, intptr_t, uintptr_t>;
        return reinterpret_cast<void *>(static_cast<intptr_type>(i));
    }

    //! "safe" cast from void* to integer
    template <typename T> T intptr_cast(void *p)
    {
        static_assert(std::is_integral<T>::value, "voidptr_cast returns an integer");
        using intptr_type = std::conditional_t<std::is_signed<T>::value, intptr_t, uintptr_t>;
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

    CMenuItem CMenu::item(std::string name, std::function<void()> callback)
    {
        assert(! name.empty());
        m_data->items->emplace_back(
            CMenuItem { m_data->id, XPLMAppendMenuItem(m_data->id, name.c_str(), voidptr_cast(m_data->items->size() + 1), false), false, false },
            [callback](bool){ callback(); }
        );
        return m_data->items->back().first;
    }

    CMenuItem CMenu::checkableItem(std::string name, bool checked, std::function<void(bool)> callback)
    {
        assert(! name.empty());
        m_data->items->emplace_back(
            CMenuItem { m_data->id, XPLMAppendMenuItem(m_data->id, name.c_str(), voidptr_cast(m_data->items->size() + 1), false), true, checked },
            callback
        );
        return m_data->items->back().first;
    }

    void CMenu::sep()
    {
        XPLMAppendMenuSeparator(m_data->id);
    }

    CMenu CMenu::subMenu(std::string name)
    {
        assert(! name.empty());
        auto items = std::make_unique<ItemList>();
        auto itemsVoidPtr = static_cast<void *>(&*items);
        return { XPLMCreateMenu(name.c_str(), m_data->id, XPLMAppendMenuItem(m_data->id, name.c_str(), nullptr, false), handler, itemsVoidPtr), false, std::move(items) };
    }

    void CMenu::handler(void *menuRef, void *itemRef)
    {
        if (menuRef && itemRef)
        {
            auto items = static_cast<ItemList *>(menuRef);
            auto itemIdx = intptr_cast<intptr_t>(itemRef) - 1;
            assert(itemIdx >= 0);

            (*items)[itemIdx].second((*items)[itemIdx].first.getChecked());
        }
    }

    CMenuItem::CMenuItem(XPLMMenuID parent, int item, bool checkable, bool checked)
        : m_data(std::make_shared<Data>(parent, item, checkable))
    {
        if (checkable)
        {
            setChecked(checked);
        }
    }

    bool CMenuItem::getChecked() const
    {
        XPLMMenuCheck check = xplm_Menu_NoCheck;
        XPLMCheckMenuItemState(m_data->parent, m_data->item, &check);
        return check == xplm_Menu_Checked;
    }

    void CMenuItem::setChecked(bool checked)
    {
        XPLMCheckMenuItem(m_data->parent, m_data->item, checked);
    }

    void CMenuItem::setEnabled(bool enabled)
    {
        XPLMEnableMenuItem(m_data->parent, m_data->item, enabled);
    }

}

//! \endcond
