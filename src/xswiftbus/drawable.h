// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef BLACKSIM_XSWIFTBUS_DRAWABLE_H
#define BLACKSIM_XSWIFTBUS_DRAWABLE_H

//! \file

#include <XPLMDisplay.h>

namespace XSwiftBus
{

    /*!
     * Class-based interface to X-Plane's drawing callback API.
     */
    class CDrawable
    {
    public:
        //! Constructor.
        CDrawable(XPLMDrawingPhase phase, bool before) : m_phase(phase), m_before(before) {}

        //! Destructor.
        virtual ~CDrawable() { hide(); }

        //! Is currently shown.
        bool isVisible() const { return m_visible; }

        //! Register the draw callback.
        void show()
        {
            if (m_visible) { return; }
            m_visible = true;
            XPLMRegisterDrawCallback(callback, m_phase, m_before, static_cast<void *>(this));
        }

        //! Unregister the draw callback.
        void hide()
        {
            if (!m_visible) { return; }
            m_visible = false;
            XPLMUnregisterDrawCallback(callback, m_phase, m_before, static_cast<void *>(this));
        }

    protected:
        //! Callback to draw the thing.
        virtual void draw() = 0;

    private:
        static int callback(XPLMDrawingPhase, int, void *refcon)
        {
            static_cast<CDrawable *>(refcon)->draw();
            return 1;
        }

        XPLMDrawingPhase m_phase;
        bool m_before = false;
        bool m_visible = false;
    };

}

#endif
