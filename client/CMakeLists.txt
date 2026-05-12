cmake_minimum_required(VERSION 3.19)
project(client LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Network Test)

qt_standard_project_setup()

qt_add_executable(client
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    client.h
    client.cpp
    loginwindow.h loginwindow.cpp loginwindow.ui
    regwindow.h regwindow.cpp regwindow.ui
)

target_link_libraries(client
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt6::Network
)

enable_testing()

qt_add_executable(client_tests
    tests/tst_client.cpp
    client.h
    client.cpp
)

target_include_directories(client_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(client_tests
    PRIVATE
        Qt::Core
        Qt6::Network
        Qt6::Test
)

add_test(NAME client_tests COMMAND client_tests)

include(GNUInstallDirs)

install(TARGETS client
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET client
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
