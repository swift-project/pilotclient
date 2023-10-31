# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

function(add_swift_test)
    set(single_args NAME)
    set(multi_args SOURCES LINK_LIBRARIES)
    cmake_parse_arguments(SWIFT_TEST "" "${single_args}" "${multi_args}" ${ARGN})

    add_executable(tests_${SWIFT_TEST_NAME} ${SWIFT_TEST_SOURCES})
    target_link_libraries(tests_${SWIFT_TEST_NAME} PUBLIC ${SWIFT_TEST_LINK_LIBRARIES})
    add_test(NAME ${SWIFT_TEST_NAME} COMMAND tests_${SWIFT_TEST_NAME})

    if(WIN32)
        add_dependencies(tests_${SWIFT_TEST_NAME} copy_externals_to_build_dir)
    endif()
endfunction()
