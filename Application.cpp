#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <iostream>
#include <vector>
#include <array>
#include <unordered_map>


// engine components
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexBufferLayout.h"
#include "VertexArray.h"
#include "Shader.h"


// math
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"


// callback functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_movement_callback(GLFWwindow* window, double xPos, double yPos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

// process all input: query GLFW function whether the relevant key are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window);
// Create a circle array
void CreateCircle(float* circle_vertices, float x, float y, float z, float radius, const int fragments);
// check if one of the figures filled cells/squares horizontally, vertically or diagonally.
void WinCheck(int figure_number, char figure, int index);

// settings
const float WIDTH = 690.0f;
const float HEIGHT = 690.0f;


namespace circle_parameters
{
	const int NUMBER_OF_SIDES = 36;
	const int NUMBER_OF_CIRCLE_LAYERS = 4;
	const int DIMENSIONS = 3;
	// multiply by 2 because each line has a two vertices.
	const int NUMBER_OF_ELEMENTS = DIMENSIONS * 2 * NUMBER_OF_CIRCLE_LAYERS * NUMBER_OF_SIDES;
}
int current_circle_layer = 0;

// figures logic
glm::mat4 translation_matrix = glm::mat4(1.0f);
int click_count = 0;
glm::vec3 last_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 position;
glm::vec3 new_position;
glm::vec3 delta_position;
std::vector<glm::mat4> positions_of_figures;
std::array<glm::vec3, 9> adjusted_positions = {
	glm::vec3(0.0f,	  0.0f,   0.0f),
	glm::vec3(0.67f,  0.0f,   0.0f),  // 0.67f is a length (width/height) of a cell/square
	glm::vec3(-0.67f, 0.0f,   0.0f),
	glm::vec3(0.0f,	  0.67f,  0.0f),
	glm::vec3(0.0f,  -0.67f,  0.0f),
	glm::vec3(0.67f,  0.67f,  0.0f),
	glm::vec3(-0.67f, 0.67f,  0.0f),
	glm::vec3(0.67f, -0.67f,  0.0f),
	glm::vec3(-0.67f, -0.67f, 0.0f)
};
std::unordered_map<glm::vec3*, int> Position_Cache;
bool win = false;
int winning_figure = -1;
std::array<int, 3> winning_positions;


int main()
{
	// GLFW: initialize and configure
	//-------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// GLFW: Create a window
	GLFWwindow* window = glfwCreateWindow(static_cast<int>(WIDTH), static_cast<int>(HEIGHT), "TicTacToe", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "WARNING: Window initialization failed" << std::endl;
		glfwTerminate();
		return -1;
	}
	// makes context of the window current for the calling thread.
	glfwMakeContextCurrent(window);


	// callback functions
	//-------------------
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_movement_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	// glad: Load all OpenGL function pointers
	//----------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "WARNING: Failed to load GLAD" << std::endl;
		glfwTerminate();
		return -1;
	}


	float grid[] = {
		// left
		-0.33f, -1.0f, 0.0f,
		-0.33f,  1.0f, 0.0f,
		// right
		 0.33f, -1.0f, 0.0f,
		 0.33f,  1.0f, 0.0f,
		// up
		-1.0f,  0.33f, 0.0f,
		 1.0f,  0.33f, 0.0f,
		// bottom
		-1.0f, -0.33f, 0.0f,
		 1.0f, -0.33f, 0.0f
	};


	float cross[] = {
		// tilted to the right
		-0.18f, -0.18f, 0.0f,
		 0.18f,  0.18f, 0.0f,
		// tilted to the left
		 0.18f, -0.18f, 0.0f,
		-0.18f,  0.18f, 0.0f,
	};


	{
		// grid
		//-----
		Shader grid_shader("resource/shaders/Basic.shader");

		VertexBuffer grid_vb(grid, sizeof(grid));
		VertexArray grid_va;
		VertexBufferLayout grid_layout;
		grid_layout.Push<float>(3);  // 3 because we have only one attribute (position vertex)
		grid_va.AddBuffer(grid_vb, grid_layout);


		// cross
		//------
		VertexBuffer cross_vb(cross, sizeof(cross));
		VertexArray cross_va;
		VertexBufferLayout cross_layout;
		cross_layout.Push<float>(3);
		cross_va.AddBuffer(cross_vb, cross_layout);


		// circle
		//-------
		float circle_vertices[circle_parameters::NUMBER_OF_ELEMENTS];
		float radius = 0.21f;
		for (int i = 0; i < circle_parameters::NUMBER_OF_CIRCLE_LAYERS; i++)
		{
			CreateCircle(circle_vertices, 0.0f, 0.0f, 0.0f, radius, circle_parameters::NUMBER_OF_SIDES);
			radius += 0.006f;
			current_circle_layer++;
		}

		VertexBuffer circle_vb(circle_vertices, sizeof(circle_vertices));
		VertexArray circle_va;
		VertexBufferLayout circle_layout;
		circle_layout.Push<float>(3);
		circle_va.AddBuffer(circle_vb, circle_layout);


		// unbind
		//-------
		grid_shader.Unbind();
		grid_vb.Unbind();
		grid_va.Unbind();

		cross_vb.Unbind();
		cross_va.Unbind();

		circle_vb.Unbind();
		circle_va.Unbind();
		//-------


		glm::mat4 grid_translation_matrix = glm::mat4(1.0f);

		Renderer renderer;

		// renderer loop
		//--------------
		while (!glfwWindowShouldClose(window))
		{
			// input
			//------
			processInput(window);

			// render
			//-------
			renderer.Clear();

			grid_shader.Bind();

			grid_vb.Bind();
			grid_va.Bind();
			grid_shader.SetUniform4f("u_color", 0.05f, 0.45f, 0.35f, 1.0f);
			grid_shader.SetUniformMat4f("translation_matrix", grid_translation_matrix);
			renderer.Draw(grid_va, grid_shader, sizeof(grid) / 3);


			// draw all currently existing figures.
			//-------------------------------------
			for (int i = 0; i < positions_of_figures.size(); i++)
			{
				if (i % 2 == 0 || i == 0)
				{
					glLineWidth(10.0f);
					cross_vb.Bind();
					cross_va.Bind();
					if (winning_figure == 0)
					{
						if (winning_positions[0] == i || winning_positions[1] == i || winning_positions[2] == i)
							grid_shader.SetUniform4f("u_color", 1.0f, 0.7f, 0.8f, 1.0f);
						else
							grid_shader.SetUniform4f("u_color", 1.0f, 1.0f, 1.0f, 1.0f);
					}
					else
						grid_shader.SetUniform4f("u_color", 1.0f, 1.0f, 1.0f, 1.0f);
					grid_shader.SetUniformMat4f("translation_matrix", positions_of_figures[i]);
					renderer.Draw(cross_va, grid_shader, sizeof(cross) / 3);
				}
				else if (i % 2 != 0)
				{
					glLineWidth(3.5f);
					circle_vb.Bind();
					circle_va.Bind();
					if (winning_figure == 1)
					{
							if (winning_positions[0] == i || winning_positions[1] == i || winning_positions[2] == i)
								grid_shader.SetUniform4f("u_color", 1.0f, 0.7f, 0.8f, 1.0f);
							else
								grid_shader.SetUniform4f("u_color", 0.0f, 0.0f, 0.0f, 0.0f);
					}
					else
						grid_shader.SetUniform4f("u_color", 0.0f, 0.0f, 0.0f, 0.0f);
					grid_shader.SetUniformMat4f("translation_matrix", positions_of_figures[i]);
					renderer.DrawCircle(circle_va, grid_shader, circle_parameters::NUMBER_OF_ELEMENTS / 3);
				}
			}
			glLineWidth(5.0f);


			// glfw: swap the buffers (front and back) to avoid flickering
			glfwSwapBuffers(window);
			// glfw: poll events (for instance, keyboard input, mouse movement, etc.)
			glfwPollEvents();
		}
	}
	glfwTerminate();
	return 0;
}


void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
	{
		click_count = 0;
		// erase (clean up (clear)) some cache where positions of x and o are stored
		positions_of_figures.clear();
		Position_Cache.clear();
		win = false;
		winning_figure = -1;
	}
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions
	GLCall(glViewport(0, 0, width, height));
}


void mouse_movement_callback(GLFWwindow* window, double xPos, double yPos)
{
	// track current position
	position = glm::vec3(static_cast<float>(xPos - (WIDTH / 2)) / (WIDTH / 2), static_cast<float>((HEIGHT / 2) - yPos) / (WIDTH / 2), 0.0f);
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	// checking if the left mouse button is pressed and it's pressed no more than 9 times
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && click_count < 9 && win == false)
	{
		for (int i = 0; i < 9; i++)
		{
			// choose the most suitable adjusted position
			if (abs(position.x - adjusted_positions[i].x) < 0.33f && abs(position.y - adjusted_positions[i].y) < 0.33f)
			{
				// Position has already been using (this cell/square is filled)
				// !! But there is no need to check for 0 value (when checking... if u_map[key] == value, value 0 is automatically filled to the place with key)
				if (Position_Cache.find(&adjusted_positions[i]) != Position_Cache.end() && Position_Cache[&adjusted_positions[i]] != 0)
				{
					std::cout << i << std::endl;
					break;
				}
				new_position = adjusted_positions[i];
				if (click_count % 2 == 0 || click_count == 0)  // cross
					Position_Cache[&adjusted_positions[i]] = click_count + 2;  // +2 to exclude 0
				else if (click_count % 2 != 0)                 // circle
					Position_Cache[&adjusted_positions[i]] = click_count + 2;

				// checking on win
				//----------------
				WinCheck(0, 'x', i);
				if (win != true)
					WinCheck(1, 'o', i);
				break;
			}
		}
		// Fill only empty cell/square
		if (new_position != last_position || click_count == 0)
		{
			click_count++;
			// find offset of the mouse
			delta_position = new_position - last_position;
			// update old position to the new position
			last_position = new_position;
			translation_matrix = glm::translate(translation_matrix, delta_position);
			positions_of_figures.push_back(translation_matrix);
			// draw
			if (positions_of_figures.size() == 9 && win == false)
				std::cout << "Draw" << std::endl;
		}
	}
}


void CreateCircle(float* circle_vertices, float x, float y, float z, float radius, const int fragments)
{
	const float doublePI = 2.0f * 3.1415926f;
	int number_of_vertices = circle_parameters::NUMBER_OF_ELEMENTS / circle_parameters::DIMENSIONS / circle_parameters::NUMBER_OF_CIRCLE_LAYERS;

	for (int i = 0; i < number_of_vertices; i++)
	{
		circle_vertices[( i + number_of_vertices * current_circle_layer) * 3] = (radius * cos(i * doublePI / fragments)) + x;  // (i * 3)
		circle_vertices[((i + number_of_vertices * current_circle_layer) * 3) + 1] = (radius * sin(i * doublePI / fragments)) + y;  // (i * 3) + 1
		circle_vertices[((i + number_of_vertices * current_circle_layer) * 3) + 2] = 0.0f + z;  // (i * 3) + 2
	}
}


void WinCheck(int mod, char figure, int i)
{
	// set matches to 0 because we include current vector
	int matches_horizontally = 0, matches_vertically = 0,
		matches_rl_diagonally = 0, matches_lr_diagonally = 0;
	// track winning positions
	std::array<int, 3> winning_horizontally, winning_vertically,
						winning_rl_diagonally, winning_lr_diagonally;

	for (int j = 0; j < 9; j++)
	{
		// horizontally
		if (Position_Cache[&adjusted_positions[i]] != 0 && Position_Cache[&adjusted_positions[j]] != 0)
		{
			if ((Position_Cache[&adjusted_positions[i]] % 2 == mod && Position_Cache[&adjusted_positions[j]] % 2 == mod)
				&& (adjusted_positions[i].y == adjusted_positions[j].y))
			{
				// store current (at the each time) number of clicks (number of clicks equals to the index of vector "positions_of_figures")
				winning_horizontally[matches_horizontally] = Position_Cache[&adjusted_positions[j]] - 2;  // -2 because origin value has been increased on 2
				matches_horizontally++;
				if (matches_horizontally == 3)
				{
					winning_positions = winning_horizontally;
					win = true;
					break;
				}
			}
			// vertically
			if ((Position_Cache[&adjusted_positions[i]] % 2 == mod && Position_Cache[&adjusted_positions[j]] % 2 == mod)
				&& (adjusted_positions[i].x == adjusted_positions[j].x))
			{
				winning_vertically[matches_vertically] = Position_Cache[&adjusted_positions[j]] - 2;
				matches_vertically++;
				if (matches_vertically == 3)
				{
					winning_positions = winning_vertically;
					win = true;
					break;
				}
			}
			// diagonally
			if ((Position_Cache[&adjusted_positions[i]] % 2 == mod && Position_Cache[&adjusted_positions[j]] % 2 == mod)
				&& (adjusted_positions[j].x == -adjusted_positions[j].y))
			{
				winning_lr_diagonally[matches_lr_diagonally] = Position_Cache[&adjusted_positions[j]] - 2;
				matches_lr_diagonally++;
				if (matches_lr_diagonally == 3)
				{
					winning_positions = winning_lr_diagonally;
					win = true;
					break;
				}
			}
			if ((Position_Cache[&adjusted_positions[i]] % 2 == mod && Position_Cache[&adjusted_positions[j]] % 2 == mod)
				&& (adjusted_positions[j].x == adjusted_positions[j].y))
			{
				winning_rl_diagonally[matches_rl_diagonally] = Position_Cache[&adjusted_positions[j]] - 2;
				matches_rl_diagonally++;
				if (matches_rl_diagonally == 3)
				{
					winning_positions = winning_rl_diagonally;
					win = true;
					break;
				}
			}
		}
	}
	if (win == true)
		winning_figure = mod;
}
