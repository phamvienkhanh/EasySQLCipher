cmake_minimum_required(VERSION 3.14)

project(EasySQLCipher LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_PRECOMPILED_HEADER OFF)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

include_directories(
    include
    inl
    vendors/sqlcipher/dist/include
)

set(HEADER include/easysqlcipher.h
           include/columndata.h
           include/precompiled.h
           include/dbomember.h
           include/dborelationmember.h
           include/dboregister.h
           include/abstractdbo.h
           include/dbhelper.h
           include/dbhelper_decl.h
           include/dbhelper_query.h
           include/dbset.h
           include/types.h
           include/sqltemplate.h
)

set(INL inl/dbhelper_fetchbyquery.inl
        inl/dbhelper_fetchbyquery_withrelation.inl
        inl/dbhelper_clausebuilder.inl
)

set(SOURCE source/easysqlcipher.cpp
           source/columndata.cpp
           source/dbomember.cpp
           source/abstractdbo.cpp
           source/dbhelper.cpp
           source/dbset.cpp
)

add_library(EasySQLCipher STATIC
  ${SOURCE} ${INL} ${HEADER}
)
link_directories(vendors/sqlcipher/dist/lib)

if(ENABLE_PRECOMPILED_HEADER)
    add_definitions(-DENABLE_PRECOMPILED_HEADER)
    target_precompile_headers(EasySQLCipher PUBLIC include/precompiled.h)
endif()

target_link_libraries(EasySQLCipher PRIVATE Qt${QT_VERSION_MAJOR}::Core sqlcipher)
target_compile_definitions(EasySQLCipher PRIVATE EASYSQLCIPHER_LIBRARY)



# for test
add_executable(TestSource source/test.h source/test.cpp)
target_link_libraries(TestSource EasySQLCipher Qt${QT_VERSION_MAJOR}::Core)
