/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

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

        //! Register the draw callback.
        void show()
        {
            XPLMRegisterDrawCallback(callback, m_phase, m_before, static_cast<void*>(this));
        }

        //! Unregister the draw callback.
        void hide()
        {
            XPLMUnregisterDrawCallback(callback, m_phase, m_before, static_cast<void*>(this));
        }

    protected:
        //! Callback to draw the thing.
        virtual void draw() = 0;

    private:
        static int callback(XPLMDrawingPhase, int, void *refcon)
        {
            static_cast<CDrawable*>(refcon)->draw();
            return 1;
        }

        const XPLMDrawingPhase m_phase;
        const bool m_before;
    };

}

#endif
