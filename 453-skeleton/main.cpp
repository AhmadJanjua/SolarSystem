//#include <GL/glew.h>
#include <glad/glad.h>
#include <imgui/imgui.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <limits>
#include <functional>

#include "Geometry.h"
#include "GLDebug.h"
#include "Log.h"
#include "ShaderProgram.h"
#include "Shader.h"
#include "Texture.h"
#include "Window.h"
#include "Camera.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "chrono"

// Constants
#define PI 3.14159265359f
#define PI2 6.28318530718f

// Reset the canvas to black before rendering anything
void reset_canvas() {

}

// Camera Manipulation ***changed to left press from right and ignored press if over imgui***
class ControlCallback : public CallbackInterface {
public:
	ControlCallback()
		: camera(glm::radians(0.f), glm::radians(0.f), 10.0)
		, aspect(1.0f)
		, l_mouse_down(false)
		, mouse_old(0.f)
	{}

	virtual void keyCallback(int key, int scancode, int action, int mods) {}

	virtual void mouseButtonCallback(int button, int action, int mods) {
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			if (action == GLFW_PRESS and !ImGui::GetIO().WantCaptureMouse) {
				l_mouse_down = true;
			}
			else if (action == GLFW_RELEASE) {
				l_mouse_down = false;
			}
		}
	}

	virtual void cursorPosCallback(double xpos, double ypos) {
		if (l_mouse_down) {
			camera.incrementTheta(ypos - mouse_old.y);
			camera.incrementPhi(xpos - mouse_old.x);
		}
		mouse_old = { xpos, ypos };
	}

	virtual void scrollCallback(double xoffset, double yoffset) {
		camera.incrementR(yoffset);
	}

	virtual void windowSizeCallback(int width, int height) {
		CallbackInterface::windowSizeCallback(width,  height);
		aspect = float(width)/float(height);
	}

	void viewPipeline(ShaderProgram &sp, ShaderProgram& sp2) {
		// camera setup
		GLint uniform;
		glm::mat4 M = glm::mat4(1.0);
		glm::mat4 V = camera.getView();
		glm::mat4 P = glm::perspective(glm::radians(45.0f), aspect, 0.01f, 1000.f);
		glm::vec3 cam_pos = camera.getPos();
		glm::vec3 light_pos = glm::vec3(P * V * M * glm::vec4(camera.getPos(), 1.f));

		// Hacky way of updating all the uniforms
		sp.use();
		uniform = glGetUniformLocation(sp, "M");
		glUniformMatrix4fv(uniform, 1, GL_FALSE, glm::value_ptr(M));
		uniform = glGetUniformLocation(sp, "V");
		glUniformMatrix4fv(uniform, 1, GL_FALSE, glm::value_ptr(V));
		uniform = glGetUniformLocation(sp, "P");
		glUniformMatrix4fv(uniform, 1, GL_FALSE, glm::value_ptr(P));
		uniform = glGetUniformLocation(sp, "camera_pos");
		glUniform3fv(uniform, 1, glm::value_ptr(cam_pos));
		uniform = glGetUniformLocation(sp, "light_pos");
		glUniform3fv(uniform, 1, glm::value_ptr(light_pos));

		// Hacky way of updating all the uniforms
		sp2.use();
		uniform = glGetUniformLocation(sp2, "M");
		glUniformMatrix4fv(uniform, 1, GL_FALSE, glm::value_ptr(M));
		uniform = glGetUniformLocation(sp2, "V");
		glUniformMatrix4fv(uniform, 1, GL_FALSE, glm::value_ptr(V));
		uniform = glGetUniformLocation(sp2, "P");
		glUniformMatrix4fv(uniform, 1, GL_FALSE, glm::value_ptr(P));
	}

private:
	Camera camera;
	float aspect;

	bool l_mouse_down;
	glm::vec2 mouse_old;
};

// Class to create a parametric sphere
class Sphere {
public:
	Sphere() {
		// temporary matrices
		std::vector<std::vector <glm::vec3 >> mat_v;
		std::vector<std::vector <glm::vec2 >> mat_t;

		// increment rate
		float inc = 0.01;

		for (float v = 0; v <= PI2;) {
			// create temporary arrays
			std::vector<glm::vec3> vert;
			std::vector<glm::vec2> text;

			for (float u = 0; u <= PI;) {
				// normal position
				vert.push_back({
					sin(u) * cos(v),
					cos(u),
					sin(u) * sin(v)
					});

				text.push_back({ 1.f - v / (PI2), 1.f - u / (PI) });

				// increment u
				u = (u < PI && u + inc > PI) ? PI : u + inc;
			}

			mat_v.push_back(vert);
			mat_t.push_back(text);

			// increment v
			v = (v < PI2 && v + inc > PI2) ? PI2 : v + inc;
		}

		for (int i = 0; i < mat_v.size() - 1; i++) {
			for (int j = 0; j < mat_v[0].size() - 1; j++) {
				// VERTS
				cgeom.verts.push_back(mat_v[i][j]);
				cgeom.verts.push_back(mat_v[i + 1][j]);
				cgeom.verts.push_back(mat_v[i + 1][j + 1]);
				cgeom.verts.push_back(mat_v[i][j]);
				cgeom.verts.push_back(mat_v[i][j + 1]);
				cgeom.verts.push_back(mat_v[i + 1][j + 1]);
				// NORMALS
				cgeom.normals.push_back(mat_v[i][j]);
				cgeom.normals.push_back(mat_v[i + 1][j]);
				cgeom.normals.push_back(mat_v[i + 1][j + 1]);
				cgeom.normals.push_back(mat_v[i][j]);
				cgeom.normals.push_back(mat_v[i][j + 1]);
				cgeom.normals.push_back(mat_v[i + 1][j + 1]);
				// TEXTURE
				cgeom.textures.push_back(mat_t[i][j]);
				cgeom.textures.push_back(mat_t[i + 1][j]);
				cgeom.textures.push_back(mat_t[i + 1][j + 1]);
				cgeom.textures.push_back(mat_t[i][j]);
				cgeom.textures.push_back(mat_t[i][j + 1]);
				cgeom.textures.push_back(mat_t[i + 1][j + 1]);
			}
		}
		ggeom.setVerts(cgeom.verts);
		ggeom.setNormals(cgeom.normals);
		ggeom.setTextures(cgeom.textures);
	}

	void bind() {
		ggeom.bind();
	}

	void draw() {
		glDrawArrays(GL_TRIANGLES, 0, GLsizei(cgeom.verts.size()));
	}

private:
	GPU_Geometry ggeom;
	CPU_Geometry cgeom;
};

// Parametric description of what defines a celestial body
struct Celestial {
	Texture& texture;
	// body
	float scale;
	float b_tilt;
	float b_rotation;
	// orbit
	float distance;
	float o_tilt;
	float o_rotation;
};

// create transformation matrixes -> render the render object
glm::mat4 drawCelestial(ShaderProgram& sp, Sphere& sphere, Celestial c, glm::mat4 M=glm::mat4(1.f)) {
	sphere.bind();
	c.texture.bind();

	GLint uniform;
	glm::mat4 identity(1.f);

	// Scaling matrix
	uniform = glGetUniformLocation(sp, "scale");
	glUniform1f(uniform, c.scale);

	// Body matrix
	uniform = glGetUniformLocation(sp, "Mb");
	// perform tilt along z axis
	glm::mat4 Mb = glm::rotate(identity, glm::radians(float(c.b_tilt)), glm::vec3(0.f, 0.f, 1.f));
	// rotate along y axis
	Mb = glm::rotate(Mb, glm::radians(float(c.b_rotation)), glm::vec3(0.f, 1.f, 0.f));
	glUniformMatrix4fv(uniform, 1, GL_FALSE, &Mb[0][0]);

	// Orbit matrix
	uniform = glGetUniformLocation(sp, "Mo");
	// axis tilt along z
	glm::mat4 Mo = glm::rotate(identity, glm::radians(float(c.o_tilt)), glm::vec3(0.f, 0.f, 1.f));
	// orbit rotation around the y axis
	Mo = glm::rotate(Mo, glm::radians(float(c.o_rotation)), glm::vec3(0.f, 1.f, 0.f));
	// translate distance from center
	Mo = glm::translate(Mo, glm::vec3(c.distance, 0.f, 0.f));
	// NOTE: since the face should not lock to the center, translate to new position instead of rotating there
	// translate + compound if there is additional movement
	Mo = glm::translate(M, glm::vec3(Mo * glm::vec4(0.f, 0.f, 0.f, 1.f)));
	glUniformMatrix4fv(uniform, 1, GL_FALSE, &Mo[0][0]);

	sphere.draw();

	// return the orbit translation
	return Mo;
}

int main() {
	glfwInit();


	// Window
	Window window(800, 800, "CPSC 453 - Assignment 4");
	GLDebug::enable();

	// Setup Imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window.getGLFWwindow(), false);
	ImGui_ImplOpenGL3_Init();


	// Callback
	auto control_cb = std::make_shared<ControlCallback>();
	window.setCallbacks(control_cb);

	// Shaders
	ShaderProgram phong("shaders/phong.vert", "shaders/phong.frag");
	ShaderProgram ambient("shaders/ambient.vert", "shaders/ambient.frag");

	// Load Textures: from https://www.solarsystemscope.com/textures/ (all of them)
	std::cout << "[INFO]: Reading star texture\t(1/4)" << std::endl;
	Texture text_space("textures/galaxy.jpg", GL_NEAREST);

	std::cout << "[INFO]: Reading sun texture\t(2/4)" << std::endl;
	Texture text_sun("textures/sun.jpg", GL_LINEAR);

	std::cout << "[INFO]: Reading earth texture\t(3/4)" << std::endl;
	Texture text_earth("textures/earth.jpg", GL_NEAREST);

	std::cout << "[INFO]: Reading moon texture\t(4/4)" << std::endl;
	Texture text_moon("textures/moon.jpg", GL_NEAREST);
	std::cout << "[INFO]: Complete!" << std::endl;

	// Geometries
	Sphere sphere;

	Celestial space {
		text_space,
		500,	// scale
		0,		// body tilt
		0,		// body rotation
		0,		// orbit distance
		0,		// orbit tilt
		0		// orbit rotation
	};

	Celestial sun {
		text_sun,
		3,		// scale
		45,		// body tilt
		0,		// body rotation
		0,		// orbit distance
		0,		// orbit tilt
		0		// orbit rotation
	};

	Celestial earth {
		text_earth,
		2,		// scale
		-25,	// body tilt
		0,		// body rotation
		9,		// orbit distance - from sun
		30,		// orbit tilt
		0		// orbit rotation
	};

	Celestial moon{
		text_moon,
		1,		// scale
		-15,	// body tilt
		0,		// body rotation
		4,		// orbit distance - from earth
		-20,	// orbit tilt
		0		// orbit rotation
	};

	// define types - readability
	typedef std::chrono::high_resolution_clock clock;
	typedef std::chrono::microseconds microseconds;

	// set rate parameters
	auto start_t = clock::now();
	const float rate_constant = 0.00001;
	float multiplier = 1;
	bool resume = true;

	// define the orbits that other bodies depend on
	glm::mat4 earth_orbit;

	// RENDER LOOP
	while (!window.shouldClose()) {
		// poll inputs
		glfwPollEvents();

		// calculate rate
		auto now_t = clock::now();
		float elapsed = std::chrono::duration_cast<microseconds>(now_t - start_t).count();
		start_t = now_t;
		float step = multiplier * rate_constant * elapsed;

		// reset the canvas to black
		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_FRAMEBUFFER_SRGB);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL /*GL_LINE*/);
		glDisable(GL_FRAMEBUFFER_SRGB); // disable sRGB for things like imgui


		// animation
		if (resume) {
			sun.b_rotation = fmod((sun.b_rotation + 1.00f * step),360);
			earth.b_rotation = fmod(earth.b_rotation + 10.0f * step, 360);
			moon.b_rotation = fmod(moon.b_rotation	+ 20.0f * step, 360);

			// Orbit speed
			earth.o_rotation = fmod(earth.o_rotation + 0.1f * step, 360);
			moon.o_rotation = fmod(moon.o_rotation	+ 1.0f * step, 360);
		}

		// update view matrices
		control_cb->viewPipeline(phong, ambient);

		// render ambient stuff
		ambient.use();
		drawCelestial(ambient, sphere, space);
		drawCelestial(ambient, sphere, sun);

		// render phong stuff
		phong.use();
		earth_orbit = drawCelestial(phong, sphere, earth);
		drawCelestial(phong, sphere, moon, earth_orbit);

		// Draw ImGui
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (ImGui::Begin("## Control")) {
			ImGui::Text("FPS %.2f", 1000000.f / elapsed);

			// Control the Speed
			ImGui::Text("Speed Multiplier");
			ImGui::SliderFloat("## Multiplier", &multiplier, 0.01f, 100);

			// Play or pause the animation
			if (ImGui::Button("Play/Pause")) {
				resume = !resume;
			}

			// reset the position of the bodies
			if (ImGui::Button("Reset")) {
				sun.b_rotation	 = 0;
				earth.b_rotation = 0;
				earth.o_rotation = 0;
				moon.b_rotation  = 0;
				moon.o_rotation  = 0;
			}
		}

		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		window.swapBuffers();
	}

	// cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();

	return 0;
}
