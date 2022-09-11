# Allow dev to specify a local Skeleton lib
# If it's not defined then just CPM it down
if(SKELETON_SOURCE_PATH STREQUAL "")
    message("No local Skeleton path specified, downloading now")

    if(SKELETON_VERSION STREQUAL "")
        CPMAddPackage(
            NAME Skeleton
            GITHUB_REPOSITORY WiggleWizard/Skeleton
            GIT_TAG master)
    else()
        message("Getting Skeleton v${SKELETON_VERSION}")
        CPMAddPackage(
            NAME Skeleton
            GITHUB_REPOSITORY WiggleWizard/Skeleton
            VERSION ${SKELETON_VERSION})
    endif()
else()
    add_subdirectory(${SKELETON_SOURCE_PATH})
endif()