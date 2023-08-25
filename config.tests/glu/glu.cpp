// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#if defined(Q_OS_WIN)
    #include <windows.h>
    #include <GL/glu.h>
#elif defined(Q_OS_MAC)
    #include <OpenGL/glu.h>
#else
    #include <GL/glu.h>
#endif

int main(int, char **)
{
    gluGetString(GL_VERSION);
    return 0;
}
