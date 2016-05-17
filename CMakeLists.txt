cmake_minimum_required(VERSION 3.1)

#find_package(Boost COMPONENTS system unit_test_framework REQUIRED)

include_directories (include)

set (CMAKE_CXX_STANDARD 11)

add_definitions(-DBOOST_TEST_DYN_LINK)

add_executable (nmeatest src/TestNMEA.cpp src/NMEA.cpp src/NMEASentences.cpp src/NMEASentenceOperators.cpp)

target_link_libraries(nmeatest boost_system boost_unit_test_framework)

enable_testing()
add_test(NMEATest nmeatest)