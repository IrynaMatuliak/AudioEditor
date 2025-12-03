cmake_minimum_required(VERSION 3.16)

project(testsSuite LANGUAGES CXX)

enable_testing()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Gui Test)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui Test)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(effects_CFadeEffect tst_effects_cfadeeffect.cpp)
add_executable(effects_CFadeInEffect tst_effects_cfadeineffect.cpp)
add_executable(effects_CFadeOutEffect tst_effects_cfadeouteffect.cpp)
add_executable(effects_CEchoEffect tst_effects_cechoeffect.cpp)
add_executable(effects_CEqualizerEffect tst_effects_cequalizereffect.cpp)
add_executable(effects_CNoiseReductionEffect tst_effects_cnoisereductioneffect.cpp)

add_test(NAME effects_CFadeEffect COMMAND effects_CFadeEffect)
add_test(NAME effects_CFadeInEffect COMMAND effects_CFadeInEffect)
add_test(NAME effects_CFadeOutEffect COMMAND effects_CFadeOutEffect)
add_test(NAME effects_CEchoEffect COMMAND effects_CEchoEffect)
add_test(NAME effects_CEqualizerEffect COMMAND effects_CEqualizerEffect)
add_test(NAME effects_CNoiseReductionEffect COMMAND effects_CNoiseReductionEffect)

target_link_libraries(effects_CFadeEffect PRIVATE Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Test AudioEffects)
target_link_libraries(effects_CFadeInEffect PRIVATE Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Test AudioEffects)
target_link_libraries(effects_CFadeOutEffect PRIVATE Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Test AudioEffects)
target_link_libraries(effects_CEchoEffect PRIVATE Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Test AudioEffects)
target_link_libraries(effects_CEqualizerEffect PRIVATE Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Test AudioEffects)
target_link_libraries(effects_CNoiseReductionEffect PRIVATE Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Test AudioEffects)
