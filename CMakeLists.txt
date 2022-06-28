cmake_minimum_required(VERSION 3.14)

project(EasySQLCipher LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

include_directories(
    include
    vendors/sqlcipher/dist/include
)

set(HEADER include/easysqlcipher.h
           include/columndata.h
           include/precompiled.h
           include/dbomember.h
           include/dboregister.h
           include/abstractdbo.h
           include/dbhelper.h
           include/dbset.h
           include/types.h           
)

set(SOURCE source/easysqlcipher.cpp
           source/columndata.cpp
           source/dbomember.cpp
           source/abstractdbo.cpp
           source/dbhelper.cpp
           source/dbset.cpp
)

add_library(EasySQLCipher STATIC
  ${SOURCE} ${HEADER}
)

link_directories(vendors/sqlcipher/dist/lib)

target_precompile_headers(EasySQLCipher PUBLIC include/precompiled.h)

target_link_libraries(EasySQLCipher PRIVATE Qt${QT_VERSION_MAJOR}::Core sqlcipher)

target_compile_definitions(EasySQLCipher PRIVATE EASYSQLCIPHER_LIBRARY)


add_executable(TestSource source/test.h source/test.cpp)
target_link_libraries(TestSource EasySQLCipher Qt${QT_VERSION_MAJOR}::Core)
