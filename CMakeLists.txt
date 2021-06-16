cmake_minimum_required(VERSION 3.0)
project(CGFire)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

include_directories(include/)



#ASSIMP laden
#umfangreiche Asset Importer Bibliothek
option(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
option(ASSIMP_BUILD_SAMPLES OFF)
option(ASSIMP_BUILD_TESTS OFF)
option(BUILD_SHARED_LIBS OFF)
add_subdirectory(vendor/assimp)
include_directories(vendor/assimp/include/)

#glad laden
#ein OpenGl loader, Header-only
include_directories(vendor/glad/include/)
add_library(gladLibrary STATIC vendor/glad/src/glad.c)


#GLFW laden
#zur Window-Erzeugung
option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory(vendor/glfw)
include_directories(vendor/glfw/include/)

#GLM laden
#ist eine OpenGL-konforme Mathe Bibliothek, Header Only
include_directories(vendor/glm)

#ImGui laden
#ist eine GUI die auch header only ist
file(GLOB imguiSOURCES vendor/imgui/*.cpp)
add_library(imGUILibrary STATIC
        ${imguiSOURCES})
include_directories(vendor/imgui/)

#STB laden
#stb ist eine Header-only Library, d.h. man muss sie nur inkludieren
#https://github.com/nothings/stb#stb
#u.a. für image loading/writing/resizing nützlich
include_directories(vendor/stb/)


#in dieser Variable befinden sich alle zu linkenden Vendor Bibliotheken
set(VENDOR_LIBRARIES
        assimp
        gladLibrary
        glfw
        ${GLFW_LIBRARIES}
        imGUILibrary)



# frameworkLibrary
# diese ist erstmal nur das gegebene framework
# und wird mit allen vendor libraries verlinkt
file(GLOB_RECURSE frameworkSOURCES src/framework/*.cpp)
add_library(frameworkLibrary STATIC ${frameworkSOURCES})
target_link_libraries(frameworkLibrary ${VENDOR_LIBRARIES})


# alles was zu unserer engine gehört
file(GLOB_RECURSE engineSOURCES
        src/engine/*.cpp
        src/particle/*.cpp
        src/util/*.cpp
        src/RiggingAndBlending/*.cpp
        src/MotionBlur/motionblur.cpp)
add_library(engineLibrary STATIC ${engineSOURCES})
target_link_libraries(engineLibrary frameworkLibrary)



#die Project-Main
add_executable(project src/project/main.cpp)
target_link_libraries(project engineLibrary)


file(GLOB TESTS_EXECUTABLES
        src/Tests/*.cpp)

#folgendes fügt für jede Datei aus TESTS_EXECUTABLES eine Executable hinzu
# und linkt alle Executables mit der engineLibrary
foreach(PROJECT_SOURCE_FILE ${TESTS_EXECUTABLES})
    get_filename_component(SRC_NAME ${PROJECT_SOURCE_FILE} NAME_WE)
    add_executable(${SRC_NAME} ${PROJECT_SOURCE_FILE} )
    target_link_libraries(${SRC_NAME} engineLibrary)
endforeach(PROJECT_SOURCE_FILE)



file(GLOB AUFGABEN_EXECUTABLES
        src/Aufgaben/*.cpp)

#folgendes fügt für jede Datei aus AUFGABEN_EXECUTABLES eine Executable hinzu
# und linkt alle Executables mit der frameworkLibrary
foreach(PROJECT_SOURCE_FILE ${AUFGABEN_EXECUTABLES})
    get_filename_component(SRC_NAME ${PROJECT_SOURCE_FILE} NAME_WE)
    add_executable(${SRC_NAME} ${PROJECT_SOURCE_FILE} include/MotionBlur/motionblur.hpp src/MotionBlur/motionblur.cpp src/Tests/Test_MotionBlur.cpp)
    target_link_libraries(${SRC_NAME} frameworkLibrary)
endforeach(PROJECT_SOURCE_FILE)



