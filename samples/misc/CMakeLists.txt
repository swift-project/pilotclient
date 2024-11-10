# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

add_executable(samples_misc
        main.cpp
        samplemisc.h
        samplesalgorithm.cpp
        samplesalgorithm.h
        sampleschangeobject.cpp
        sampleschangeobject.h
        samplescontainer.cpp
        samplescontainer.h
        samplesfile.cpp
        samplesfile.h
        samplesjson.cpp
        samplesjson.h
        samplesmetadata.cpp
        samplesmetadata.h
        samplesperformance.cpp
        samplesperformance.h
)
target_link_libraries(samples_misc core misc Qt::Core)
