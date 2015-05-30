#include "../include/gl_all.h"
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
#include "shader.vert"
;
const std::string fshader_source = 
#include "shader.frag"
;

//for skymap texture

const std::string vshader_sky =
#include "simple2.vert"
;

const std::string fshader_sky =
#include "simple2.frag"
;

const GLfloat floorvertex[][3] = 
{
	{ 5.0f,  0.0f,  5.0f},
	{ 5.0f,  0.0f, -5.0f},
	{-5.0f,  0.0f, -5.0f},
	{-5.0f,  0.0f,  5.0f}
};

const GLfloat floornormal[][3] = 
{
	{ 0.0f,  1.0f,  0.0f},
	{ 0.0f,  1.0f,  0.0f},
	{ 0.0f,  1.0f,  0.0f},
	{ 0.0f,  1.0f,  0.0f}
};

const GLfloat floortexcrd[][2] =
{
	{1.0f, 1.0f},
	{0.0f, 1.0f},
	{0.0f, 0.0f},
	{1.0f, 0.0f}
};

const GLushort floor_index[] =
{
	0,1,2,0,2,3
};

//various parameter

const float RANGE_X = 200;
const float RANGE_Y = 200;
const float RANGE_Z = 200;

int width = 1200;
int height = 800;

float curr_x = 200;
float curr_y = 150;
float curr_z = -200;
float curr_theta = 2*M_PI/3.0f;
float curr_phi = 0.0f;
float accel = 5.0f;
float velocity = 0.0f;
bool flag_l = false;

Uint32 tick = 0;

float light_speed = 100.0f;


int main(int argc, char* argv[])
{
	using namespace jikoLib::GLLib;

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


	SDL_Window* window = SDL_CreateWindow("SDL_Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 200, 200, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	//SDL_Window* window = SDL_CreateWindow("SDL_Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1200, 800, SDL_WINDOW_OPENGL);
	if(window == NULL)
	{
		std::cerr << "Window could not be created!: " << SDL_GetError() << std::endl;
	}

	SDL_GLContext context;

	context = SDL_GL_CreateContext(window);

	obj << Begin();

	SDL_GL_SetSwapInterval(1);

	SDL_GL_MakeCurrent(window, context);

	VShader vshader;
	FShader fshader;

	ShaderProgram program;

	vshader << vshader_source;
	fshader << fshader_source;

	program << vshader << fshader << link_these();

	ShaderProgram skyprogram;

	VShader vshader1;
	FShader fshader1;

	vshader1 << vshader_sky;
	fshader1 << fshader_sky;

	skyprogram << vshader1 << fshader1 << link_these();
	

	//
	
	//texture
	Texture<Texture2D> texture;
	texture.texImage2D("texture.jpg");
	texture.setParameter<Wrap_S<GL_REPEAT>, Wrap_T<GL_REPEAT>, Wrap_R<GL_REPEAT>, Mag_Filter<GL_NEAREST>, Min_Filter<GL_NEAREST>>();

	Texture<Texture2D> texture2;
	texture2.texImage2D("texture2.jpg");
	texture.setParameter<Wrap_S<GL_REPEAT>, Wrap_T<GL_REPEAT>, Wrap_R<GL_REPEAT>, Mag_Filter<GL_NEAREST>, Min_Filter<GL_NEAREST>>();

	Texture<Texture2D> texture3;
	texture3.texImage2D("texture3.jpg");
	texture.setParameter<Wrap_S<GL_REPEAT>, Wrap_T<GL_REPEAT>, Wrap_R<GL_REPEAT>, Mag_Filter<GL_NEAREST>, Min_Filter<GL_NEAREST>>();
	
	//floor mesh
	
	std::vector<Mesh3D> floor_mesh_arr;

	for(float z = -200; z<200; z+=10)
	{
		for(float x = -200; x<200; x+=10)
		{
			Mesh3D floor_mesh;
			floor_mesh.copyData(floorvertex, floornormal, floortexcrd);
			floor_mesh.copyIndex(floor_index);
			floor_mesh.setPos(glm::vec3(x, 0.0f, z));
			floor_mesh_arr.push_back(floor_mesh);
		}
	}

	//cubes
	
	std::vector<Mesh3D> cube_arr;
	MeshSample::Cube c_helper(10);

	for(float x=-100; x<110; x+=10)
	{
		Mesh3D cube_mesh;
		cube_mesh.copyData(c_helper.getVertex(), c_helper.getNormal(), c_helper.getTexcrd(), c_helper.getNumVertex());
		cube_mesh.setPos(glm::vec3(x, 5.0f, -100.0f));
		cube_arr.push_back(cube_mesh);
	}

	for(float y=15; y<100; y+=10)
	{
		Mesh3D cube_mesh;
		cube_mesh.copyData(c_helper.getVertex(), c_helper.getNormal(), c_helper.getTexcrd(), c_helper.getNumVertex());
		cube_mesh.setPos(glm::vec3(100.0f, y, -100.0f));
		cube_arr.push_back(cube_mesh);
	}

	for(float y=15; y<100; y+=10)
	{
		Mesh3D cube_mesh;
		cube_mesh.copyData(c_helper.getVertex(), c_helper.getNormal(), c_helper.getTexcrd(), c_helper.getNumVertex());
		cube_mesh.setPos(glm::vec3(-100.0f, y, -100.0f));
		cube_arr.push_back(cube_mesh);
	}

	for(float x=-100; x<110; x+=10)
	{
		Mesh3D cube_mesh;
		cube_mesh.copyData(c_helper.getVertex(), c_helper.getNormal(), c_helper.getTexcrd(), c_helper.getNumVertex());
		cube_mesh.setPos(glm::vec3(x, 100.0f, -100.0f));
		cube_arr.push_back(cube_mesh);
	}


	for(float x=-100; x<110; x+=10)
	{
		Mesh3D cube_mesh;
		cube_mesh.copyData(c_helper.getVertex(), c_helper.getNormal(), c_helper.getTexcrd(), c_helper.getNumVertex());
		cube_mesh.setPos(glm::vec3(x, 5.0f, 100.0f));
		cube_arr.push_back(cube_mesh);
	}

	for(float y=15; y<100; y+=10)
	{
		Mesh3D cube_mesh;
		cube_mesh.copyData(c_helper.getVertex(), c_helper.getNormal(), c_helper.getTexcrd(), c_helper.getNumVertex());
		cube_mesh.setPos(glm::vec3(100.0f, y, 100.0f));
		cube_arr.push_back(cube_mesh);
	}

	for(float y=15; y<100; y+=10)
	{
		Mesh3D cube_mesh;
		cube_mesh.copyData(c_helper.getVertex(), c_helper.getNormal(), c_helper.getTexcrd(), c_helper.getNumVertex());
		cube_mesh.setPos(glm::vec3(-100.0f, y, 100.0f));
		cube_arr.push_back(cube_mesh);
	}

	for(float x=-100; x<110; x+=10)
	{
		Mesh3D cube_mesh;
		cube_mesh.copyData(c_helper.getVertex(), c_helper.getNormal(), c_helper.getTexcrd(), c_helper.getNumVertex());
		cube_mesh.setPos(glm::vec3(x, 100.0f, 100.0f));
		cube_arr.push_back(cube_mesh);
	}


	//sphere
	

	std::vector<Mesh3D> sphere_arr;
	MeshSample::Sphere sp_helper(10, 30, 30);

	Mesh3D sphere_mesh;
	sphere_mesh.copyData(sp_helper.getVertex(), sp_helper.getNormal(), sp_helper.getTexcrd(), sp_helper.getNumVertex());
	sphere_mesh.setPos(glm::vec3(-100.0f, 50.0f, 0.0f));
	sphere_arr.push_back(sphere_mesh);
	sphere_mesh.setPos(glm::vec3(0.0f, 50.0f, 0.0f));
	sphere_arr.push_back(sphere_mesh);
	sphere_mesh.setPos(glm::vec3(100.0f, 50.0f, 0.0f));
	sphere_arr.push_back(sphere_mesh);
	sphere_mesh.setPos(glm::vec3(-100.0f, 150.0f, 0.0f));
	sphere_arr.push_back(sphere_mesh);
	sphere_mesh.setPos(glm::vec3(0.0f, 150.0f, 0.0f));
	sphere_arr.push_back(sphere_mesh);
	sphere_mesh.setPos(glm::vec3(100.0f, 150.0f, 0.0f));
	sphere_arr.push_back(sphere_mesh);




	//skymap mesh
	//Mesh3D skymap_mesh;
	//MeshSample::CubeMap cube(1024);
	//skymap_mesh.copyData(cube.getVertex(), cube.getNormal(), cube.getTexcrd(), cube.getNumVertex());

	//skymap texture
	//Texture<TextureCubeMap> texture_sky;
	//texture_sky.texImage2D("cubeStar_right.png", "cubeStar_left.png", "cubeStar_front.png", "cubeStar_back.png", "cubeStar_bottom.png", "cubeStar_top.png");
	//texture_sky.texImage2D("negx.jpg", "posx.jpg", "negy.jpg", "posy.jpg", "negz.jpg", "posz.jpg");
	//texture_sky.setParameter<Mag_Filter<GL_LINEAR>, Min_Filter<GL_LINEAR>>();
	//texture_sky.generateMipmap();

	//camera
	Camera camera;
	camera.setUp(glm::vec3(0.0f, 1.0f, 0.0f));
	camera.setFar(50000L);

	// TODO: add some shapes

	// TODO: add cubemap
	



	bool quit = false;
	SDL_Event e;
	//	SDL_WaitThread(threadID, NULL);

	while( !quit )
	{
		//Handle events on queue
		while( SDL_PollEvent( &e ) != 0 )
		{
			//User requests quit
			if( e.type == SDL_QUIT )
			{
				quit = true;
			}
			// window resize event
			if(e.type == SDL_WINDOWEVENT)
			{
				switch(e.window.event)
				{
					case SDL_WINDOWEVENT_SIZE_CHANGED:
						width = e.window.data1;
						height = e.window.data2;
						break;
				}
			}
			else if(e.type == SDL_KEYDOWN)
			{
				switch (e.key.keysym.sym)
				{
					case SDLK_z:
						flag_l = flag_l?false:true;
						break;
					default:
						break;
				}
			}

			

		}

		//upload pos
		//
		int x, y;
		Uint32 flag = SDL_GetMouseState(&x, &y);

		curr_phi = (3*M_PI-0.01)*((double)x/width);
		curr_theta = (M_PI-0.005)*((double)y/height);

		Uint32 interval;
		if(tick == 0)
		{
			interval = 16;
		}
		else
		{
			interval = SDL_GetTicks() - tick;
		}

		tick = SDL_GetTicks();

		if(flag & SDL_BUTTON(SDL_BUTTON_LEFT))
		{
			//left button pushed
			velocity += 2*accel*interval/1000.0f;
			if(velocity > light_speed - 0.03f) velocity = light_speed-0.03f;
		}
		else if(flag & SDL_BUTTON(SDL_BUTTON_RIGHT))
		{
			velocity -= 4*accel*interval/1000.0f;
			if(velocity < 0.0f) velocity = 0.0f;
		}
		else
		{
			velocity -= accel*interval/1000.0f;
			if(velocity < 0.0f) velocity = 0.0f;
		}

		curr_x += sin(curr_theta)*sin(curr_phi)*(interval/1000.0f)*velocity;
		curr_y += cos(curr_theta)*(interval/1000.0f)*velocity;
		curr_z += sin(curr_theta)*cos(curr_phi)*(interval/1000.0f)*velocity;

		if(curr_x < -RANGE_X) curr_x = -RANGE_X;
		if(curr_x >  RANGE_X) curr_x =  RANGE_X;
		if(curr_y < 5) curr_y = 5;
		if(curr_y >  2*RANGE_Y) curr_y =  2*RANGE_Y;
		if(curr_z < -RANGE_Z) curr_z = -RANGE_Z;
		if(curr_z >  RANGE_Z) curr_z =  RANGE_Z;


		obj.clearColor(0.0f, 0.0f, 0.0f, 1.0f);
		obj.clearDepth(1.0);
		obj.viewport(0,0,width,height);
		obj.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		// TODO: setting camera direction

		camera.setDrct(glm::vec3(curr_x, curr_y, curr_z)+glm::vec3(sin(curr_theta)*sin(curr_phi), cos(curr_theta), sin(curr_theta)*cos(curr_phi)));
		camera.setAspect(width, height);
		camera.setPos(glm::vec3(curr_x, curr_y, curr_z));
		program.setUniformMatrixXtv("view", glm::value_ptr(camera.getViewMatrix()), 1, 4);
		program.setUniformMatrixXtv("projection", glm::value_ptr(camera.getProjectionMatrix()), 1, 4);

		// TODO: render all shapes

		//light speed
		if(flag_l)
		{
			program.setUniformXt("beta", velocity/light_speed);
			std::cout << "v/c = " << velocity/light_speed << std::endl;
		}
		else
		{
			program.setUniformXt("beta", 0.0f);
			std::cout << "v/c = OFF"  << std::endl;
		}

		// light setting

		program.setUniformXt("light.ambient", 0.75f, 0.75f, 0.75f, 1.0f);
		program.setUniformXt("light.diffuse", 0.7f, 0.7f, 0.7f, 1.0f);
		program.setUniformXt("light.specular", 1.0f, 1.0f, 1.0f, 1.0f);
		program.setUniformXt("light.position", curr_x, curr_y, curr_z);
		program.setUniformXt("diminish", 1.0f);

		//floor with texture
		program.setUniformXt("textureobj", 0);
		
		program.setUniformXt("material.ambient", 0.3f, 0.25f, 0.4f, 1.0f);
		program.setUniformXt("material.diffuse", 0.75f, 0.0f, 1.0f, 1.0f);
		program.setUniformXt("material.specular", 1.0f, 1.0f, 1.0f, 1.0f);
		program.setUniformXt("material.shininess", 32.0f);

		program.setUniformXt("attenuation.constant", 0.00f);
		program.setUniformXt("attenuation.linear", 0.2f);
		program.setUniformXt("attenuation.quadratic", 0.0f);

		for(auto& floor_mesh : floor_mesh_arr)
		{
			obj.connectAttrib(program, floor_mesh, "vertex", "normal", "texcrd");
			program.setUniformMatrixXtv("model", glm::value_ptr(floor_mesh.getModelMatrix()), 1, 4);
			texture3.bind(0);
			obj.draw(floor_mesh, program);
			texture3.unbind();
		}

		for(auto& cube_mesh : cube_arr)
		{
			obj.connectAttrib(program, cube_mesh, "vertex", "normal", "texcrd");
			program.setUniformMatrixXtv("model", glm::value_ptr(cube_mesh.getModelMatrix()), 1, 4);
			texture2.bind(0);
			obj.draw(cube_mesh, program);
			texture2.unbind();
		}

		for(auto& sphere_mesh : sphere_arr)
		{
			obj.connectAttrib(program, sphere_mesh, "vertex", "normal", "texcrd");
			program.setUniformMatrixXtv("model", glm::value_ptr(sphere_mesh.getModelMatrix()), 1, 4);
			texture.bind(0);
			obj.draw(sphere_mesh, program);
			texture.unbind();
		}



		//skymap
		//skymap_mesh.setPos(glm::vec3(curr_x, curr_y, curr_z));
		//skyprogram.setUniformMatrixXtv("model", glm::value_ptr(skymap_mesh.getModelMatrix()), 1, 4);
		//skyprogram.setUniformMatrixXtv("view", glm::value_ptr(camera.getViewMatrix()), 1, 4);
		//skyprogram.setUniformMatrixXtv("projection", glm::value_ptr(camera.getProjectionMatrix()), 1, 4);
		//skyprogram.setUniformXt("textureobj", 0);
		//obj.connectAttrib(skyprogram, skymap_mesh, "vertex");

		//texture_sky.bind(0);
		//obj.draw(skymap_mesh, skyprogram);
		//texture_sky.unbind();


		SDL_GL_SwapWindow( window );
	}

	//	obj << End();

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
