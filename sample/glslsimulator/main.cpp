/*******************************************************************************
 * OpenGLLib
 *
 * The MIT License (MIT) * 
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
#include "../include/animlib/anim_all.h"
#include <vector>
#include <SDL2/SDL.h>
#include <IL/ilu.h>
#include <SDL2/SDL_opengl.h>
#include <fstream>

jikoLib::GLLib::GLObject obj;

const std::string vshader_source = 
#include "glsimu.vert"
;
const std::string fshader_source = 
#include "glsimu.frag"
;


const int WIDTH = 512;
const int HEIGHT = 512;

using namespace jikoLib::GLLib;
using namespace jikoLib::AnimLib;

ShaderProgram getProgram(const std::string &v, const std::string &f)
{
	VShader vs;
	FShader fs;
	ShaderProgram prog;
	vs << v;
	fs << f;
	prog << vs << fs << link_these();
	return prog;
}


int main()
{

	/*************************************
	 * SDL initialize
	 * **********************************/

	if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		std::cerr << "Cannot Initialize SDL!: " << SDL_GetError() << std::endl;
		return -1;
	}


	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); 


	/*
	SDL_Rect window_rect;
	if(SDL_GetDisplayBounds(0, &window_rect) != 0)
	{
		std::cerr << "SDL_GetDisplayBounds failed" << std::endl;
		return 0;
	}
	*/
	
	SDL_Window* window = SDL_CreateWindow("SDL_Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
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

	SDL_GL_SetSwapInterval(0);

	SDL_GL_MakeCurrent(window, context);

	//load shaders
	ShaderProgram program = getProgram(vshader_source, fshader_source);




	//vertex buffer
	float vertex_data[][3] = 
	{
		{-1,  1, 0},
		{-1, -1, 0},
		{ 1,  1, 0},
		{ 1, -1, 0},
	};
	VBO vertex;
	vertex.copyData(vertex_data);

	//vertex array
	VAO varray;

	//front texture
	Texture<Texture2D> front_texture;
	front_texture.texImage2D(WIDTH, HEIGHT);
	front_texture.setParameter<Mag_Filter<GL_LINEAR>, Min_Filter<GL_LINEAR>, Wrap_S<GL_REPEAT>, Wrap_T<GL_REPEAT>>();

	//renderbuffer
	//RBO front_rend;
	//front_rend.storage<DepthComponent16>(WIDTH, HEIGHT);

	//framebuffer
	FBO fFront;
	fFront.attach<ColorAttachment<0>>(front_texture);
	//fFront.attach<DepthAttachment>(front_rend);

	//back texture
	Texture<Texture2D> back_texture;
	back_texture.texImage2D(WIDTH, HEIGHT);
	back_texture.setParameter<Mag_Filter<GL_LINEAR>, Min_Filter<GL_LINEAR>, Wrap_S<GL_REPEAT>, Wrap_T<GL_REPEAT>>();

	//renderbuffer
	//RBO back_rend;
	//back_rend.storage<DepthComponent16>(WIDTH, HEIGHT);

	//framebuffer
	FBO fBack;
	fBack.attach<ColorAttachment<0>>(back_texture);
	//fBack.attach<DepthAttachment>(back_rend);
	
	//crate main fragment shader
	//load main fragment shader
	std::ifstream ifs("shader.frag");
	if(ifs.fail())
	{
		std::cerr << "load fragment shader failed" << std::endl;
	}
	std::string main_fshader_source((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

	ShaderProgram main_prog = getProgram(vshader_source, main_fshader_source);
	ifs.clear();
	ifs.close();

	//for timer
	float time = 0;


	FpsFix fps(61, 1);

	//draw
	volatile bool quit = false;
	SDL_Event e;
	while(!quit)
	{
		while(SDL_PollEvent(&e) != 0)
		{
			if(e.type == SDL_QUIT)
				quit = true;
			if(e.type == SDL_KEYDOWN)
			{
				int key = e.key.keysym.sym;
				if(key == SDLK_r)
				{
					//reload main fragment shader
					ifs.open("shader.frag");
					if(ifs.fail())
					{
						std::cerr << "load fragment shader failed" << std::endl;
					}
					std::string main_fshader_source((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
					main_prog = getProgram(vshader_source, main_fshader_source);
					ifs.clear();
					ifs.close();
				}
			}
		}

		fps.frameStart();

		//draw to front framebuffer
		obj.connectAttrib(main_prog, vertex, varray, "position");
		obj.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, fFront);
		obj.clearColor(0.0, 0.0, 0.0, 1.0, fFront);
		obj.viewport(0, 0, WIDTH, HEIGHT, fFront);
		
		//mouse position
		int mouse[2];
		SDL_GetMouseState(&(mouse[0]), &(mouse[1]));
		float mouse_f[2];
		mouse_f[0] = (float)mouse[0]*2.0/WIDTH-1.0; mouse_f[1] = 1.0-(float)mouse[1]*2.0/HEIGHT;
		main_prog.setUniformXtv("m", mouse_f);

		//time
		main_prog.setUniformXt("t", time);
		time += 1.0/fps.getFps();

		//resolution
		float resolution[] = {WIDTH, HEIGHT};
		main_prog.setUniformXtv("r", resolution);

		//sampler
		main_prog.setUniformXt("smp", 0);

		//draw
		fFront.bind();
		back_texture.bind(0);
		if(fps.isDrawable())
			obj.draw<rm_TriangleStrip>(varray, main_prog, vertex);
		back_texture.unbind();
		fFront.unbind();

		//draw above to main buffer
		obj.connectAttrib(program, vertex, varray, "position");
		obj.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		obj.clearColor(0.0, 0.0, 0.0, 1.0);
		obj.viewport(0, 0, WIDTH, HEIGHT);
		program.setUniformXt("texture", 0);
		front_texture.bind(0);
		if(fps.isDrawable())
			obj.draw<rm_TriangleStrip>(varray, program, vertex);
		front_texture.unbind();

		//swap framebuffer and texture
		FBO fTemp;
		Texture<Texture2D> temp_texture;

		fTemp = fFront;
		temp_texture = front_texture;
		fFront = fBack;
		front_texture = back_texture;
		fBack = fTemp;
		back_texture = temp_texture;

		SDL_GL_SwapWindow(window);

		fps.frameEnd();
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
