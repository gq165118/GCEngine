cmake_minimum_required(VERSION 3.12)
project(GCEngine_Project)

set(CMAKE_CXX_STANDARD 17)

file(GLOB copyResources "./assets" "./thirdParty/libs/assimp/assimp-vc143-mtd.dll")
file(COPY ${copyResources} DESTINATION ${CMAKE_BINARY_DIR})

include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/thirdParty/include)

link_directories(
	SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/thirdParty/libs/glfw
	SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/thirdParty/libs/assimp
)

add_subdirectory(ff)

add_executable(triangle "examples/triangle.cpp" )
add_executable(test "examples/test.cpp" )

#target_link_libraries(dianosaurScene ff_lib  glfw3.lib assimp-vc143-mtd.lib)
target_link_libraries(triangle ff_lib  glfw3.lib assimp-vc143-mtd.lib)
#target_link_libraries(cube ff_lib  glfw3.lib assimp-vc143-mtd.lib)
#target_link_libraries(directionalLight ff_lib  glfw3.lib assimp-vc143-mtd.lib)
#target_link_libraries(materials ff_lib  glfw3.lib assimp-vc143-mtd.lib)
#target_link_libraries(modelLoading ff_lib  glfw3.lib assimp-vc143-mtd.lib)
#target_link_libraries(animation ff_lib  glfw3.lib assimp-vc143-mtd.lib)
#arget_link_libraries(renderTarget ff_lib  glfw3.lib assimp-vc143-mtd.lib)