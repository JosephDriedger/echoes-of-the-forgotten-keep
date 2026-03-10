cmake_minimum_required(VERSION 4.1)

file(GLOB_RECURSE EFK_TEST_SOURCES CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

if(EFK_TEST_SOURCES)
    add_executable(efk_tests
            ${EFK_TEST_SOURCES}
    )

    target_link_libraries(efk_tests
            PRIVATE
            efk_engine
            efk_game
    )

    target_compile_features(efk_tests PRIVATE cxx_std_17)

    add_test(NAME efk_tests COMMAND efk_tests)

    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES
            ${EFK_TEST_SOURCES}
    )
endif()