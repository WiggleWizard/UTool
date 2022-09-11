set(PROJECT_SOURCE_FILES
    "Main.cpp"
    "UToolApplication.cpp"
    "Tools/ToolCookedMapsManager.cpp")

set(PROJECT_LINK_DIRS
    ${PROJECT_SOURCE_DIR}/include)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_FILES} ${PROJECT_RESOURCES} ${EASTL_SOURCE_DIR}/doc/EASTL.natvis)
target_include_directories(${PROJECT_NAME} PRIVATE . ${PROJECT_LINK_DIRS} ${PROJECT_RESOURCES_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME}
    Skeleton::Module::Core
    Skeleton::Module::OS
    Skeleton::Module::ImGui
    
    spdlog::spdlog

    simpleini)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
