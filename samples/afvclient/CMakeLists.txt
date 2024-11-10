# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

add_executable(samples_afvclient main.cpp afvclientbridge.h afvclientbridge.cpp qml/qml.qrc)
target_link_libraries(samples_afvclient PUBLIC core misc config Qt::Core Qt::Qml Qt::Gui)
target_compile_definitions(samples_afvclient PRIVATE _USE_MATH_DEFINES)
