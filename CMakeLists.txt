cmake_minimum_required(VERSION 3.10)
project(Project-3-Game-Recommender)

set(CMAKE_CXX_STANDARD 11)

add_executable(Project-3-Game-Recommender src/main.cpp src/GameStruct.cpp src/HashTable.cpp)

include_directories(include)

set(DATASET_TSV "${CMAKE_CURRENT_SOURCE_DIR}/src/dataset.tsv")
set(GAME_JSON "${CMAKE_CURRENT_SOURCE_DIR}/src/game.json")

add_custom_command(
        TARGET Project-3-Game-Recommender POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${DATASET_TSV}
        $<TARGET_FILE_DIR:Project-3-Game-Recommender>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${GAME_JSON}
        $<TARGET_FILE_DIR:Project-3-Game-Recommender>
)

# Ensure the default command line copy if building in Visual Studio
if(MSVC)
    set_property(
            DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            PROPERTY VS_STARTUP_PROJECT Project-3-Game-Recommender
    )
endif()
