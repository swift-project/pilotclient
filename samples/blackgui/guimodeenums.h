/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef SAMPLE_GUIMODEENUMS_H
#define SAMPLE_GUIMODEENUMS_H

//! Modes, how GUI can be started (core/GUI)
struct GuiModes {

public:
    //! Window modes
    enum WindowMode {
        WindowFrameless,
        WindowNormal
    };

    //! Core runs how and where?
    enum CoreMode {
        CoreInGuiProcess,
        CoreExternal,
        CoreExternalAudioLocal
    };
};

#endif // guard
