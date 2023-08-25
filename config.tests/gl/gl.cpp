// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#if defined(Q_OS_WIN)
    #include <windows.h>
    #include <GL/gl.h>
#elif defined(Q_OS_MAC)
    #include <OpenGL/gl.h>
#else
    #include <GL/gl.h>
#endif

int main(int, char **)
{
    glGetString(GL_VERSION);
    return 0;
}
