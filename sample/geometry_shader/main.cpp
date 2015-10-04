/*******************************************************************************
 * OpenGLLib
 *
 * The MIT License (MIT)
 * 
 * Copyright (c) 2015 j-i-k-o
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 *******************************************************************************/

#include "../include/gllib/gl_all.h"
#include <vector>
#include <SDL2/SDL.h>
#include <IL/ilu.h>
#include <SDL2/SDL_opengl.h>

jikoLib::GLLib::GLObject obj;

int threadfunction(void* data)
{
	using namespace jikoLib::GLLib;

	return 0;
}

const std::string vshader_source = 
#include "geom.vert"
;
const std::string gshader_source = 
#include "geom.geom"
;
const std::string fshader_source = 
#include "geom.frag"
;


int main(int argc, char* argv[])
{
	using namespace jikoLib::GLLib;


	/*************************************
	 * SDL initialize
	 * **********************************/

	if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		std::cerr << "Cannot Initialize SDL!: " << SDL_GetError() << std::endl;
		return -1;
	}

	//SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
	//SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
	//SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
	//SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); 


	SDL_Rect window_rect;
	if(SDL_GetDisplayBounds(0, &window_rect) != 0)
	{
		std::cerr << "SDL_GetDisplayBounds failed" << std::endl;
		return 0;
	}
	SDL_Window* window = SDL_CreateWindow("SDL_Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window_rect.w, window_rect.h, SDL_WINDOW_OPENGL);
	if(window == NULL)
	{
		std::cerr << "Window could not be created!: " << SDL_GetError() << std::endl;
	}

	SDL_GLContext context;

	context = SDL_GL_CreateContext(window);

	/***************************************
	 * 
	 * ************************************/

	obj << Begin();

	SDL_GL_SetSwapInterval(1);

	SDL_GL_MakeCurrent(window, context);

	//load shaders
	VShader vshader;
	GShader gshader;
	FShader fshader;

	ShaderProgram program;
	vshader << vshader_source;
	gshader << gshader_source;
	fshader << fshader_source;

	program << vshader << gshader << fshader << link_these();

	Mesh3D cube;
	MeshSample::Cube cubeHelper(1.0);
	cube.copyData(cubeHelper.getVertex(), cubeHelper.getNormal(), cubeHelper.getTexcrd(), cubeHelper.getNumVertex());

	Camera camera;
	camera.setPos(glm::vec3(3.0f, 3.0f, -2.0f));
	camera.setDrct(glm::vec3(0.0f, 0.0f, 0.0f));
	camera.setUp(glm::vec3(0.0f, 1.0f, 0.0f));
	camera.setFar(10.0f);

	//window size
	obj.connectAttrib(program, cube.getNormal(), cube.getVArray(), "norm");
	obj.connectAttrib(program, cube.getVertex(), cube.getVArray(), "vertex");
	obj.connectAttrib(program, cube.getTexcrd(), cube.getVArray(), "texcrd");

	program.setUniformMatrixXtv("model", glm::value_ptr(cube.getModelMatrix()), 1, 4);
	//program.setUniformMatrixXtv("view", glm::value_ptr(camera.getViewMatrix()), 1, 4);
	//program.setUniformMatrixXtv("projection", glm::value_ptr(camera.getProjectionMatrix()), 1, 4);

	program.setUniformXt("light.ambient", 0.25f, 0.25f, 0.25f, 1.0f);
	program.setUniformXt("light.diffuse", 1.0f, 1.0f, 1.0f, 1.0f);
	program.setUniformXt("light.specular", 1.0f, 1.0f, 1.0f, 1.0f);

	program.setUniformXt("material.ambient", 0.3f, 0.0f, 0.4f, 1.0f);
	program.setUniformXt("material.diffuse", 0.75f, 0.0f, 1.0f, 1.0f);
	program.setUniformXt("material.specular", 1.0f, 1.0f, 1.0f, 1.0f);
	program.setUniformXt("material.shininess", 3.0f);

	program.setUniformXt("attenuation.constant", 0.0f);
	program.setUniformXt("attenuation.linear", 0.5f);
	program.setUniformXt("attenuation.quadratic", 0.075f);

	program.setUniformXt("textureobj", 0);

	//draw
	volatile bool quit = false;
	SDL_Event e;
	while(!quit)
	{
		while(SDL_PollEvent(&e) != 0)
		{
			if(e.type == SDL_QUIT)
				quit = true;
		}
		CHECK_GL_ERROR;
		obj.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		obj.clearColor(0.0f, 0.0f, 0.0f, 1.0f);
		program.setUniformXt("light.position", 0.6f, 0.6f, -0.6f);
		int width, height;
		SDL_GetWindowSize(window, &width, &height);
		camera.setAspect(width, height);
		program.setUniformMatrixXtv("view", glm::value_ptr(camera.getViewMatrix()), 1, 4);
		program.setUniformMatrixXtv("projection", glm::value_ptr(camera.getProjectionMatrix()), 1, 4);
		obj.viewport(0, 0, width, height);
		obj.draw(cube, program);
		SDL_GL_SwapWindow(window);
	}


	
	/*************************************
	 * SDL desctuction
	 * **********************************/

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	/***************************************
	 * 
	 * ************************************/

	return 0;
}
