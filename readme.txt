CPSC 453: Assignment 4
Name: Ahmad Janjua
UCID: 30118441

Controls and Usage: mouse and ImGui
- Camera
	- instead of right click to press using left click to press
	- hovering over the ImGui panel will prevent you from moving camera
	- rest of the implementation left standard

- Animation
	- slider to change speed
	- pause and play button
	- reset the animation state button

Compiling:
- CMake file was not modified
- Extra headers include
	- #include <imgui/imgui.h>
	- #include <imgui/imgui_impl_glfw.h>
	- #include <imgui/imgui_impl_opengl3.h>
	- #include "chrono"

Files Modifications:
	- Window.h - get a GLFWwindow pointer for ImGui (inline definition)
	- Geometry - got rid of color and used texture instead
	- main.cpp
	- ambient.vert
	- ambient.frag
	- phong.vert
	- phong.frag

System Description:
- Windows 11
- Visual Studio 2022
- Compiler: Default (CPP 17)