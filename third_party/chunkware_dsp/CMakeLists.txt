# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

add_library(ChunkwareDsp STATIC)
target_sources(ChunkwareDsp
        PRIVATE
        SimpleComp.cpp
        SimpleComp.h
        SimpleCompProcess.inl
        SimpleEnvelope.cpp
        SimpleEnvelope.h
        SimpleGain.h
        SimpleGate.cpp
        SimpleGate.h
        SimpleGateProcess.inl
        SimpleHeader.h
        SimpleLimit.cpp
        SimpleLimit.h
        SimpleLimitProcess.inl
)
target_include_directories(ChunkwareDsp
        PUBLIC
        ${PROJECT_SOURCE_DIR}/third_party
)
set_property(TARGET ChunkwareDsp PROPERTY POSITION_INDEPENDENT_CODE ON)
set_source_files_properties(SimpleComp.cpp PROPERTIES SKIP_LINTING ON)
set_source_files_properties(SimpleEnvelope.cpp PROPERTIES SKIP_LINTING ON)
set_source_files_properties(SimpleGate.cpp PROPERTIES SKIP_LINTING ON)
set_source_files_properties(SimpleLimit.cpp PROPERTIES SKIP_LINTING ON)
