/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SAMPLE_GUIMODEENUMS_H
#define SAMPLE_GUIMODEENUMS_H

struct GuiModes {

public:
    /*!
     * \brief Window mode
     */
    enum WindowMode {
        WindowFrameless,
        WindowNormal
    };

    /*!
     * \brief Core runs how and where?
     */
    enum CoreMode {
        CoreInGuiProcess,
        CoreExternal,
        CoreExternalVoiceLocal
    };
};

#endif // guard
