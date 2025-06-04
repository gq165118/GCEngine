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
