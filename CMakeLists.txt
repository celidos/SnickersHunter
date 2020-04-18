# CMake entry point
cmake_minimum_required (VERSION 2.6)
project (Tutorials)

find_package(OpenGL REQUIRED)


if( CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR )
    message( FATAL_ERROR "Please select another Build Directory ! (and give it a clever name, like bin_Visual2012_64bits/)" )
endif()
if( CMAKE_SOURCE_DIR MATCHES " " )
	message( "Your Source Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()
if( CMAKE_BINARY_DIR MATCHES " " )
	message( "Your Build Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()



# Compile external dependencies 
add_subdirectory (external)

# On Visual 2005 and above, this module can set the debug working directory
# cmake_policy(SET CMP0026 OLD)
# list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/external/rpavlik-cmake-modules-fe2273")
# include(CreateLaunchers)
# include(MSVCMultipleProcessCompile) # /MP

if(INCLUDE_DISTRIB)
	add_subdirectory(distrib)
endif(INCLUDE_DISTRIB)



include_directories(
	external/AntTweakBar-1.16/include/
	external/glfw-3.1.2/include/GLFW/
	external/glm-0.9.7.1/
	external/glew-1.13.0/include/
	external/assimp-3.0.1270/include/
	external/bullet-2.81-rev2613/src/
	.
)

set(ALL_LIBS
	${OPENGL_LIBRARY}
	glfw
	GLEW_1130
)

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)


# Tutorial 2
add_executable(hunter 
	hunter/main.cpp
	common/shader.cpp
	common/shader.hpp
	common/controls.cpp
	common/controls.hpp
	common/texture.cpp
	common/texture.hpp
	
	hunter/TransformVertexShader.vertexshader
	hunter/TextureFragmentShader.fragmentshader
	
	hunter/SimpleFragmentShader.fragmentshader
	hunter/SimpleVertexShader.vertexshader
)
target_link_libraries(hunter
	${ALL_LIBS}
)
# Xcode and Visual working directories
set_target_properties(hunter PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/hunter/")
#create_target_launcher(hunter WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/hunter/")
#create_default_target_launcher(hunter WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/hunter/") # tut 1 is not the default or people would complain that tut 2 doesn't work



#Task1
add_executable(task1
	task1/task1.cpp
	common/shader.cpp
	common/shader.hpp
    
	task1/GreenFragmentShader.fragmentshader
    	task1/RedFragmentShader.fragmentshader
	task1/SimpleVertexShader.vertexshader
    
)
target_link_libraries(task1
	${ALL_LIBS}
)
# Xcode and Visual working directories
set_target_properties(task1 PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/task1/")
#create_target_launcher(task1 WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/task1/")
#create_default_target_launcher(task1 WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/task1/") # tut 1 is not the default or people would complain that tut 2 doesn't work

#Task2
add_executable(task2
	task2/task2.cpp
	common/shader.cpp
	common/shader.hpp
	
	task2/GreenFragmentShader.fragmentshader
    	task2/RedFragmentShader.fragmentshader
	task2/SimpleVertexShader.vertexshader
)
target_link_libraries(task2
	${ALL_LIBS}
)
# Xcode and Visual working directories
set_target_properties(task2 PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/task2/")
#create_target_launcher(task2 WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/task2/")
#create_default_target_launcher(task2 WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/task2/") # tut 1 is not the default or people would complain that tut 2 doesn't work



#Task3
add_executable(task3
	task3/task3.cpp
	common/shader.cpp
	common/shader.hpp
	
	task3/SimpleFragmentShader.fragmentshader
	task3/SimpleVertexShader.vertexshader
)
target_link_libraries(task3
	${ALL_LIBS}
)
# Xcode and Visual working directories
set_target_properties(task3 PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/task3/")
#create_target_launcher(task3 WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/task3/")
#create_default_target_launcher(task3 WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/task3/") # tut 1 is not the default or people would complain that tut 2 doesn't work





SOURCE_GROUP(common REGULAR_EXPRESSION ".*/common/.*" )
SOURCE_GROUP(shaders REGULAR_EXPRESSION ".*/.*shader$" )


if (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )
add_custom_command(
   TARGET hunter POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/hunter${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/hunter/"
)
add_custom_command(
   TARGET task1 POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/task1${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/task1/"
)
add_custom_command(
   TARGET task2 POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/task2${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/task2/"
)
add_custom_command(
   TARGET task3 POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/task3${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/task3/"
)
elseif (${CMAKE_GENERATOR} MATCHES "Xcode" )

endif (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )

