
include_directories(
	C:\\sdk\\glfw-3.3.8.bin.WIN64\\include
	C:\\sdk\\glew-2.1.0\\include
	C:\\sdk\\freeglut\\include
)

link_directories(
	C:\\sdk\\glfw-3.3.8.bin.WIN64\\lib-vc2022
	C:\\Windows\\System32
	C:\\sdk\\glew-2.1.0\\lib\\Release\\x64
	C:\\sdk\\freeglut\\lib\\x64
)



# Add source to this project's executable.
add_executable (MC "MC.cpp" )

#add_executable (test "test.cpp")

target_link_libraries(MC glfw3.lib opengl32 glew32 freeglut)
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET MC PROPERTY CXX_STANDARD 20)
endif()


