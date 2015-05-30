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

#pragma once

#include "gl_base.h"
#include "gl_debug.h"
#include "gl_3D.h"
#include <functional>

namespace jikoLib{
	namespace GLLib {


		class GLObject
		{
			private:
				//member variable
				bool _is_initialized;

			public:
				GLObject() :_is_initialized(false)
			{
			}
				bool initialize()
				{
					bool success = true;
					//initialize glew
					glewExperimental = GL_TRUE;
					GLenum glewError = glewInit();
					if(glewError != GLEW_OK)
					{
						std::cerr << "cannot initialize GLEW!: " << glewGetErrorString(glewError) << std::endl;
						success = false;
					}
					else
					{
						//OpenGL initialize complete
						DEBUG_OUT("--- OpenGL initialize complete ---");
						DEBUG_OUT("OpenGL Vendor: " << glGetString(GL_VENDOR));
						DEBUG_OUT("OpenGL Renderer: " << glGetString(GL_RENDERER));
						DEBUG_OUT("OpenGL Version: " << glGetString(GL_VERSION));
						DEBUG_OUT(" ");
						_is_initialized = true;
					}
					ilInit();
					iluInit();
					ILenum ilerror = ilGetError();
					if(ilerror != IL_NO_ERROR)
					{
						std::cerr << "cannot initialize devIL!: " << iluErrorString(ilerror) << std::endl; 
					}
					else
					{
						DEBUG_OUT("--- devIL initialize complete ---");
						ilClearColour(255, 255, 255, 0);
					}
					return success;
				}

				/*
					void finalize(const std::function<void()> &fin_func)
					{
					if(_is_initialized)
					{
					fin_func();
					DEBUG_OUT("OpenGL Context is closed.");
					_is_initialized = false;
					}
					}
					*/

				/*
					void makeCurrent(const std::function<void()> &makec_func)
					{
					makec_func();
					DEBUG_OUT("Make Current");
					}
					*/



				GLObject& operator<<(Begin&&)
					//initializer
				{
					initialize();
					return *this;
				}
				/*

					GLObject& operator<<(MakeCurrent&& obj)
				//make current
				{
				makeCurrent(obj.m_window);
				return *this;
				}

				GLObject& operator<<(End&&)
				//finalizer
				{
				finalize();
				return *this;
				}
				*/

				template<typename UsageType, typename Allocator_sh, typename Allocator_vb, typename Allocator_va>
					void connectAttrib(const ShaderProg<Allocator_sh> &prog, const VertexBuffer<ArrayBuffer, UsageType, Allocator_vb> &buffer, const VertexArray<Allocator_va> &varray, const std::string &name)
					{
						if(!buffer.getisSetArray())
						{
							std::cerr << "Array is not set! --did nothing" << std::endl;
							return;
						}
						if(getSizeof(buffer.getArrayEnum()) == 0)
						{
							std::cerr << "buffer ArrayEnum is invalid! --did nothing" << std::endl;
							return;
						}
						varray.bind();
						buffer.bind();
						GLint attribloc = glGetAttribLocation(prog.getID(), name.c_str());
						CHECK_GL_ERROR;
						if(attribloc == -1)
						{
							std::cerr << "attribute variable " << name << " cannot be found" << std::endl;
							return;
						}
						glVertexAttribPointer(attribloc, buffer.getDim(), buffer.getArrayEnum(), GL_FALSE, buffer.getDim()*getSizeof(buffer.getArrayEnum()), 0);
						CHECK_GL_ERROR;
						glEnableVertexAttribArray(attribloc);
						CHECK_GL_ERROR;
						buffer.unbind();
						varray.unbind();
					}

				template<typename Allocator_sh>
					void disconnectAttrib(const ShaderProg<Allocator_sh> &prog, const std::string &name)
					{
						glDisableVertexAttribArray(glGetAttribLocation(prog.getID(), name.c_str()));
						CHECK_GL_ERROR;
					}

				template<typename Allocator_sh>
					inline void connectAttrib(const ShaderProg<Allocator_sh> &prog, const Mesh3D &mesh, const std::string &vertex_attr, const std::string &normal_attr = "", const std::string &texcrd_attr = "")
					{
						this->connectAttrib(prog, mesh.getVertex(), mesh.getVArray(), vertex_attr);
						if(normal_attr != "")
							this->connectAttrib(prog, mesh.getNormal(), mesh.getVArray(), normal_attr);
						if(texcrd_attr != "")
							this->connectAttrib(prog, mesh.getTexcrd(), mesh.getVArray(), texcrd_attr);
					}

				inline void connectAttrib(AssimpModel &model, const std::string &vertex_attr, const std::string &normal_attr = "", const std::string &texcrd_attr = "")
				{
					//set vertex normal texcrd variable to AssimpModel
					model.setVertexNormalTexcrd(vertex_attr, normal_attr, texcrd_attr);
				}


				inline void viewport(GLint x, GLint y, GLsizei width, GLsizei height)
				{
					glViewport(x,y,width,height);
				}

				template<typename fbTargetType, typename fbAllocator>
					inline void viewport(GLint x, GLint y, GLsizei width, GLsizei height,const FrameBuffer<fbTargetType, fbAllocator> &fbo)
					{
						fbo.bind();
						glViewport(x,y,width,height);
						fbo.unbind();
					}

				inline void clearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
				{
					glClearColor(red, green, blue, alpha);
				}

				template<typename fbTargetType, typename fbAllocator>
					inline void clearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha, const FrameBuffer<fbTargetType, fbAllocator> &fbo)
					{
						fbo.bind();
						glClearColor(red, green, blue, alpha);
						fbo.unbind();
					}

				inline void clearDepth(GLclampd depth)
				{
					glClearDepth(depth);
				}

				template<typename fbTargetType, typename fbAllocator>
					inline void clearDepth(GLclampd depth, const FrameBuffer<fbTargetType, fbAllocator> &fbo)
					{
						fbo.bind();
						glClearDepth(depth);
						fbo.unbind();
					}

				inline void clear(GLbitfield mask)
				{
					glClear(mask);
				}

				template<typename fbTargetType, typename fbAllocator>
					inline void clear(GLbitfield mask, const FrameBuffer<fbTargetType, fbAllocator> &fbo)
					{
						fbo.bind();
						glClear(mask);
						fbo.unbind();
					}





				//draw

				template<typename RenderMode = rm_Triangles, typename varrAlloc, typename Sp_Alloc, typename vbUsage, typename vbAlloc>
					void draw(const VertexArray<varrAlloc> &varray, const ShaderProg<Sp_Alloc> &program, const VertexBuffer<ElementArrayBuffer, vbUsage, vbAlloc> &ibo)
					{
						varray.bind();
						ibo.bind();
						program.bind();

						if(!ibo.getisSetArray())
						{
							std::cerr << "IBO array isn't set. cannot draw" << std::endl;
						}
						//else
						glDrawElements(RenderMode::RENDER_MODE, ibo.getSizeElem(), ibo.getArrayEnum(), NULL);
						CHECK_GL_ERROR;
						program.unbind();
						ibo.unbind();
						varray.unbind();
					}


				template<typename RenderMode = rm_Triangles, typename varrAlloc, typename Sp_Alloc, typename vbUsage, typename vbAlloc>
					void draw(const VertexArray<varrAlloc> &varray, const ShaderProg<Sp_Alloc> &program, const VertexBuffer<ArrayBuffer, vbUsage, vbAlloc> &vbo)
					{
						varray.bind();
						program.bind();
						if(!vbo.getisSetArray())
						{
							std::cerr << "VBO array isn't set. cannot draw" << std::endl;
						}
						//else
						glDrawArrays(RenderMode::RENDER_MODE, 0, vbo.getSizeElem());
						CHECK_GL_ERROR;
						program.unbind();
						varray.unbind();
					}



				template<typename RenderMode = rm_Triangles, typename varrAlloc, typename Sp_Alloc, typename TexTarget, typename TexAlloc, typename vbUsage, typename vbAlloc>
					void draw(
							const VertexArray<varrAlloc> &varray,
							const ShaderProg<Sp_Alloc> &program,
							const VertexBuffer<ElementArrayBuffer,
							vbUsage, vbAlloc> &ibo,
							const std::vector<std::tuple<Texture<TexTarget, TexAlloc>,
							std::size_t>> &tex_array)
					{
						for (auto&& var : tex_array) {
							std::get<0>(var).bind(std::get<1>(var));
						}

						draw<RenderMode>(varray, program, ibo);

						for(auto&& var : tex_array) {
							std::get<0>(var).unbind();
						}
					}

				template<typename RenderMode = rm_Triangles, typename varrAlloc, typename Sp_Alloc, typename TexTarget, typename TexAlloc, typename vbUsage, typename vbAlloc>
					void draw(
							const VertexArray<varrAlloc> &varray,
							const ShaderProg<Sp_Alloc> &program,
							const VertexBuffer<ArrayBuffer, vbUsage, vbAlloc> &vbo,
							const std::vector<std::tuple<Texture<TexTarget, TexAlloc>, std::size_t>> &tex_array)
					{
						for (auto&& var : tex_array) {
							std::get<0>(var).bind(std::get<1>(var));
						}

						draw<RenderMode>(varray, program, vbo);

						for(auto&& var : tex_array) {
							std::get<0>(var).unbind();
						}
					}


				template<typename RenderMode = rm_Triangles, typename Sp_Alloc>
					inline void draw(const Mesh3D &obj, const ShaderProg<Sp_Alloc> &program)
					{
						if(obj.getIsIndexSet())
							draw<RenderMode>(obj.getVArray(), program, obj.getIndex());
						else
							draw<RenderMode>(obj.getVArray(), program, obj.getVertex());
					}


				template<typename RenderMode = rm_Triangles, typename Sp_Alloc, typename TexTarget, typename TexAlloc>
					inline void draw(const Mesh3D &obj, const ShaderProg<Sp_Alloc> &program, const std::vector<std::tuple<Texture<TexTarget, TexAlloc>, std::size_t>> &tex_array)
					{
						if(obj.getIsIndexSet())
							draw<RenderMode>(obj.getVArray(), program, obj.getIndex(), tex_array);
						else
							draw<RenderMode>(obj.getVArray(), program, obj.getVertex(), tex_array);
					}

				
				
				static void recursive_render(const aiScene *sc, const aiNode* nd, const AssimpModel& model)
				{
				
				}

				inline void draw(const AssimpModel& model, const std::string& model_uniform, const glm::mat4& model_mat)
				{
					recursive_render(model.getScene(), model.getScene()->mRootNode, model);
				}
				
		};
	} 
}
