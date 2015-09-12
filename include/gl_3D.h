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

#include <cmath>
#define M_PI 3.14159265358979323846
#include <vector>
#include <fstream>
#include <tuple>
#include <string>
#include "gl_helper.h"
#include "gl_base.h"
#include "gl_debug.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing

namespace jikoLib{
	namespace GLLib{

		class Mesh3D{
			private:
				VBO vertex;
				VBO normal;
				VBO texcrd;
				IBO index;
				VAO v_array;

				glm::vec3 pos;
				glm::quat rot;
				glm::vec3 scale;

			public:

				Mesh3D()
					:pos(0.0f, 0.0f, 0.0f),
					rot(1.0f, 0.0f, 0.0f, 0.0f),
					scale(1.0f, 1.0f, 1.0f)  {}

				inline const VBO& getVertex() const
				{
					return vertex;
				}
				inline const VBO& getNormal() const
				{
					return normal;
				}
				inline const VBO& getTexcrd() const
				{
					return texcrd;
				}
				inline const IBO& getIndex() const
				{
					return index;
				}
				inline const VAO& getVArray() const
				{
					return v_array;
				}

				inline bool getIsSetArray() const
				{
					return 
						vertex.getisSetArray() &&
						normal.getisSetArray() &&
						texcrd.getisSetArray();
				}

				inline bool getIsIndexSet() const
				{
					return index.getisSetArray();
				}

				template<typename T, std::size_t Size_Elem>
					inline void copyData(const T (&vert)[Size_Elem][3], const T (&norm)[Size_Elem][3], const T (&tex)[Size_Elem][2])
					{
						vertex.copyData(vert);
						normal.copyData(norm);
						texcrd.copyData(tex );
					}

				template<typename T, std::size_t Size_Elem>
					inline void copyData(const T (&vert)[Size_Elem][3], const T (&norm)[Size_Elem][3])
					{
						vertex.copyData(vert);
						normal.copyData(norm);
					}

				template<typename T, std::size_t Size_Elem>
					inline void copyIndex(const T (&ind)[Size_Elem])
					{
						index.copyData(ind);
					}

				template<typename T>
					inline void copyData(const T *vert, const T *norm, const T *tex, std::size_t Size_Elem)
					{
						vertex.copyData(vert, Size_Elem, 3);
						normal.copyData(norm, Size_Elem, 3);
						texcrd.copyData( tex, Size_Elem, 2);
					}

				template<typename T>
					inline void copyData(const T *vert, const T *norm, std::size_t Size_Elem)
					{
						vertex.copyData(vert, Size_Elem, 3);
						normal.copyData(norm, Size_Elem, 3);
					}

				template<typename T>
					inline void copyIndex(const T *ind, std::size_t Size_Elem)
					{
						index.copyData(ind, Size_Elem);
					}

				inline void setPos(const glm::vec3 &vec)
				{
					this->pos = vec;
				}

				inline const glm::vec3& getPos()
				{
					return this->pos;
				}

				inline void setScale(const glm::vec3 &vec)
				{
					this->scale = vec;
				}

				inline const glm::vec3& getScale()
				{
					return this->scale;
				}

				inline void rotate(const glm::vec3 &axis, GLfloat angle)
				{
					this->rot = glm::rotate(this->rot, angle, glm::normalize(axis));
				}

				inline void resetRot()
				{
					this->rot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
				}

				inline const glm::quat& getRot()
				{
					return this->rot;
				}

				inline glm::mat4 getModelMatrix()
				{
					glm::mat4 mat = glm::mat4_cast(rot);
					glm::mat4 poyo = mat*glm::scale(glm::mat4(), scale);
					return glm::translate(poyo, glm::inverse(glm::mat3(poyo))*pos);
				}
		};

		class Camera{
			private:
				glm::vec3 pos; //position
				glm::vec3 drct;  //direction
				glm::vec3 up; //up

				GLfloat fovy;
				GLfloat aspect;
				GLfloat _near;
				GLfloat _far;

			public:

				Camera()
					:pos(0.0f, 0.0f, 0.0f),
					drct(1.0f, 0.0f, 0.0f),
					up(0.0f, 1.0f, 0.0f),
					fovy(M_PI/6.0),
					aspect(1.0f),
					_near(0.1f),
					_far(10.0f) {}

				inline void setPos(const glm::vec3 &pos)
				{
					this->pos = pos;
				}
				inline void setDrct(const glm::vec3 &drct)
				{
					this->drct = drct;
				}
				inline void setUp(const glm::vec3 &up)
				{
					this->up = up;
				}
				inline void setFovy(GLfloat fovy)
				{
					this->fovy = fovy;
				}
				inline void setAspect(GLfloat aspect)
				{
					this->aspect = aspect;
				}
				inline void setAspect(GLfloat width, GLfloat height)
				{
					this->aspect = width / height;
				}
				inline void setNear(GLfloat _near)
				{
					this->_near = _near;
				}
				inline void setFar(GLfloat _far)
				{
					this->_far = _far;
				}
				inline const glm::vec3& getPos() const
				{
					return this->pos;
				}
				inline const glm::vec3& getDrct() const
				{
					return this->drct;
				}

				inline glm::mat4 getViewMatrix() const
				{
					return glm::lookAt(pos, drct, up);
				}

				inline glm::mat4 getProjectionMatrix() const
				{
					return glm::perspective(fovy, aspect, _near, _far);
				}
		};

		namespace MeshSample{

			class AbstractShape{
				protected:
					std::vector<GLfloat> vertex;
					std::vector<GLfloat> normal;
					std::vector<GLfloat> texcrd;
					std::vector<GLushort> index;
				public:
					inline const GLfloat* getVertex()
					{
						return vertex.data();
					}

					inline const GLfloat* getNormal()
					{
						return normal.data();
					}

					inline const GLfloat* getTexcrd()
					{
						return texcrd.data();
					}

					inline const GLushort* getIndex()
					{
						return index.data();
					}

					inline std::size_t getNumVertex()
					{
						return vertex.size()/3;
					}

					inline std::size_t getNumNormal()
					{
						return normal.size()/3;
					}

					inline std::size_t getNumTexcrd()
					{
						return texcrd.size()/2;
					}

					inline std::size_t getNumIndex()
					{
						return index.size();
					}
			};

			class Cube : public AbstractShape{
				private:

				public:
					Cube(GLfloat side)
					{
						vertex.reserve(108);
						normal.reserve(108);
						texcrd.reserve(72);

						//forward
						vertex.push_back(-side/2);
						vertex.push_back(-side/2);
						vertex.push_back( side/2);

						vertex.push_back( side/2);
						vertex.push_back(-side/2);
						vertex.push_back( side/2);

						vertex.push_back( side/2);
						vertex.push_back( side/2);
						vertex.push_back( side/2);

						vertex.push_back( side/2);
						vertex.push_back( side/2);
						vertex.push_back( side/2);

						vertex.push_back(-side/2);
						vertex.push_back( side/2);
						vertex.push_back( side/2);

						vertex.push_back(-side/2);
						vertex.push_back(-side/2);
						vertex.push_back( side/2);

						//back
						vertex.push_back( side/2);
						vertex.push_back(-side/2);
						vertex.push_back(-side/2);

						vertex.push_back(-side/2);
						vertex.push_back(-side/2);
						vertex.push_back(-side/2);

						vertex.push_back(-side/2);
						vertex.push_back( side/2);
						vertex.push_back(-side/2);

						vertex.push_back(-side/2);
						vertex.push_back( side/2);
						vertex.push_back(-side/2);

						vertex.push_back( side/2);
						vertex.push_back( side/2);
						vertex.push_back(-side/2);

						vertex.push_back( side/2);
						vertex.push_back(-side/2);
						vertex.push_back(-side/2);

						//right
						vertex.push_back( side/2);
						vertex.push_back(-side/2);
						vertex.push_back( side/2);

						vertex.push_back( side/2);
						vertex.push_back(-side/2);
						vertex.push_back(-side/2);

						vertex.push_back( side/2);
						vertex.push_back( side/2);
						vertex.push_back(-side/2);

						vertex.push_back( side/2);
						vertex.push_back( side/2);
						vertex.push_back(-side/2);

						vertex.push_back( side/2);
						vertex.push_back( side/2);
						vertex.push_back( side/2);

						vertex.push_back( side/2);
						vertex.push_back(-side/2);
						vertex.push_back( side/2);

						//left
						vertex.push_back(-side/2);
						vertex.push_back(-side/2);
						vertex.push_back(-side/2);

						vertex.push_back(-side/2);
						vertex.push_back(-side/2);
						vertex.push_back( side/2);

						vertex.push_back(-side/2);
						vertex.push_back( side/2);
						vertex.push_back( side/2);

						vertex.push_back(-side/2);
						vertex.push_back( side/2);
						vertex.push_back( side/2);

						vertex.push_back(-side/2);
						vertex.push_back( side/2);
						vertex.push_back(-side/2);

						vertex.push_back(-side/2);
						vertex.push_back(-side/2);
						vertex.push_back(-side/2);

						//up
						vertex.push_back(-side/2);
						vertex.push_back( side/2);
						vertex.push_back( side/2);

						vertex.push_back( side/2);
						vertex.push_back( side/2);
						vertex.push_back( side/2);

						vertex.push_back( side/2);
						vertex.push_back( side/2);
						vertex.push_back(-side/2);

						vertex.push_back( side/2);
						vertex.push_back( side/2);
						vertex.push_back(-side/2);

						vertex.push_back(-side/2);
						vertex.push_back( side/2);
						vertex.push_back(-side/2);

						vertex.push_back(-side/2);
						vertex.push_back( side/2);
						vertex.push_back( side/2);

						//down
						vertex.push_back( side/2);
						vertex.push_back(-side/2);
						vertex.push_back( side/2);

						vertex.push_back(-side/2);
						vertex.push_back(-side/2);
						vertex.push_back( side/2);

						vertex.push_back(-side/2);
						vertex.push_back(-side/2);
						vertex.push_back(-side/2);

						vertex.push_back(-side/2);
						vertex.push_back(-side/2);
						vertex.push_back(-side/2);

						vertex.push_back( side/2);
						vertex.push_back(-side/2);
						vertex.push_back(-side/2);

						vertex.push_back( side/2);
						vertex.push_back(-side/2);
						vertex.push_back( side/2);

						for(int i = 0; i < 6; i++)
						{
							normal.push_back(0.0f);
							normal.push_back(0.0f);
							normal.push_back(1.0f);
						}

						for(int i = 0; i < 6; i++)
						{
							normal.push_back(0.0f);
							normal.push_back(0.0f);
							normal.push_back(-1.0f);
						}

						for(int i = 0; i < 6; i++)
						{
							normal.push_back(1.0f);
							normal.push_back(0.0f);
							normal.push_back(0.0f);
						}

						for(int i = 0; i < 6; i++)
						{
							normal.push_back(-1.0f);
							normal.push_back(0.0f);
							normal.push_back(0.0f);
						}

						for(int i = 0; i < 6; i++)
						{
							normal.push_back(0.0f);
							normal.push_back(1.0f);
							normal.push_back(0.0f);
						}

						for(int i = 0; i < 6; i++)
						{
							normal.push_back(0.0f);
							normal.push_back(-1.0f);
							normal.push_back(0.0f);
						}

						for(int i = 0; i < 6; i++)
						{
							texcrd.push_back(0.0f);
							texcrd.push_back(0.0f);
							texcrd.push_back(1.0f);
							texcrd.push_back(0.0f);
							texcrd.push_back(1.0f);
							texcrd.push_back(1.0f);
							texcrd.push_back(1.0f);
							texcrd.push_back(1.0f);
							texcrd.push_back(0.0f);
							texcrd.push_back(1.0f);
							texcrd.push_back(0.0f);
							texcrd.push_back(0.0f);
						}
					}

			};

			class CubeMap : public AbstractShape{
				private:

				public:
					CubeMap(GLfloat side)
					{
						vertex.reserve(108);
						normal.reserve(108);
						texcrd.reserve(72);

						//forward
						vertex.push_back( side/2);
						vertex.push_back( side/2);
						vertex.push_back( side/2);

						vertex.push_back( side/2);
						vertex.push_back(-side/2);
						vertex.push_back( side/2);

						vertex.push_back(-side/2);
						vertex.push_back(-side/2);
						vertex.push_back( side/2);

						vertex.push_back(-side/2);
						vertex.push_back(-side/2);
						vertex.push_back( side/2);

						vertex.push_back(-side/2);
						vertex.push_back( side/2);
						vertex.push_back( side/2);

						vertex.push_back( side/2);
						vertex.push_back( side/2);
						vertex.push_back( side/2);

						//back
						vertex.push_back(-side/2);
						vertex.push_back( side/2);
						vertex.push_back(-side/2);

						vertex.push_back(-side/2);
						vertex.push_back(-side/2);
						vertex.push_back(-side/2);

						vertex.push_back( side/2);
						vertex.push_back(-side/2);
						vertex.push_back(-side/2);

						vertex.push_back( side/2);
						vertex.push_back(-side/2);
						vertex.push_back(-side/2);

						vertex.push_back( side/2);
						vertex.push_back( side/2);
						vertex.push_back(-side/2);

						vertex.push_back(-side/2);
						vertex.push_back( side/2);
						vertex.push_back(-side/2);

						//right
						vertex.push_back( side/2);
						vertex.push_back( side/2);
						vertex.push_back(-side/2);

						vertex.push_back( side/2);
						vertex.push_back(-side/2);
						vertex.push_back(-side/2);

						vertex.push_back( side/2);
						vertex.push_back(-side/2);
						vertex.push_back( side/2);

						vertex.push_back( side/2);
						vertex.push_back(-side/2);
						vertex.push_back( side/2);

						vertex.push_back( side/2);
						vertex.push_back( side/2);
						vertex.push_back( side/2);

						vertex.push_back( side/2);
						vertex.push_back( side/2);
						vertex.push_back(-side/2);

						//left
						vertex.push_back(-side/2);
						vertex.push_back( side/2);
						vertex.push_back( side/2);

						vertex.push_back(-side/2);
						vertex.push_back(-side/2);
						vertex.push_back( side/2);

						vertex.push_back(-side/2);
						vertex.push_back(-side/2);
						vertex.push_back(-side/2);

						vertex.push_back(-side/2);
						vertex.push_back(-side/2);
						vertex.push_back(-side/2);

						vertex.push_back(-side/2);
						vertex.push_back( side/2);
						vertex.push_back(-side/2);

						vertex.push_back(-side/2);
						vertex.push_back( side/2);
						vertex.push_back( side/2);

						//up
						vertex.push_back( side/2);
						vertex.push_back( side/2);
						vertex.push_back(-side/2);

						vertex.push_back( side/2);
						vertex.push_back( side/2);
						vertex.push_back( side/2);

						vertex.push_back(-side/2);
						vertex.push_back( side/2);
						vertex.push_back( side/2);

						vertex.push_back(-side/2);
						vertex.push_back( side/2);
						vertex.push_back( side/2);

						vertex.push_back(-side/2);
						vertex.push_back( side/2);
						vertex.push_back(-side/2);

						vertex.push_back( side/2);
						vertex.push_back( side/2);
						vertex.push_back(-side/2);

						//down
						vertex.push_back(-side/2);
						vertex.push_back(-side/2);
						vertex.push_back(-side/2);

						vertex.push_back(-side/2);
						vertex.push_back(-side/2);
						vertex.push_back( side/2);

						vertex.push_back( side/2);
						vertex.push_back(-side/2);
						vertex.push_back( side/2);

						vertex.push_back( side/2);
						vertex.push_back(-side/2);
						vertex.push_back( side/2);

						vertex.push_back( side/2);
						vertex.push_back(-side/2);
						vertex.push_back(-side/2);

						vertex.push_back(-side/2);
						vertex.push_back(-side/2);
						vertex.push_back(-side/2);

						for(int i = 0; i < 6; i++)
						{
							normal.push_back(0.0f);
							normal.push_back(0.0f);
							normal.push_back(-1.0f);
						}

						for(int i = 0; i < 6; i++)
						{
							normal.push_back(0.0f);
							normal.push_back(0.0f);
							normal.push_back( 1.0f);
						}

						for(int i = 0; i < 6; i++)
						{
							normal.push_back(-1.0f);
							normal.push_back(0.0f);
							normal.push_back(0.0f);
						}

						for(int i = 0; i < 6; i++)
						{
							normal.push_back( 1.0f);
							normal.push_back(0.0f);
							normal.push_back(0.0f);
						}

						for(int i = 0; i < 6; i++)
						{
							normal.push_back(0.0f);
							normal.push_back(-1.0f);
							normal.push_back(0.0f);
						}

						for(int i = 0; i < 6; i++)
						{
							normal.push_back(0.0f);
							normal.push_back( 1.0f);
							normal.push_back(0.0f);
						}

						for(int i = 0; i < 6; i++)
						{
							texcrd.push_back(0.0f);
							texcrd.push_back(0.0f);
							texcrd.push_back(1.0f);
							texcrd.push_back(0.0f);
							texcrd.push_back(1.0f);
							texcrd.push_back(1.0f);
							texcrd.push_back(1.0f);
							texcrd.push_back(1.0f);
							texcrd.push_back(0.0f);
							texcrd.push_back(1.0f);
							texcrd.push_back(0.0f);
							texcrd.push_back(0.0f);
						}
					}

			};

			class Sphere : public AbstractShape
			{
				private:
				public:
					Sphere(GLfloat radius, int SLICES, int STACKS)
					{
						vertex.reserve(SLICES*(STACKS-1)*18);
						normal.reserve(SLICES*(STACKS-1)*18);
						texcrd.reserve(SLICES*(STACKS-1)*12);

						for(int i = 0; i < SLICES; i++)
						{
							vertex.push_back(0);
							vertex.push_back(0);
							vertex.push_back(radius);

							texcrd.push_back((GLfloat)i/SLICES);
							texcrd.push_back(1.0f);

							vertex.push_back(radius*sin(M_PI*(GLfloat)(1)/STACKS)*cos(2*M_PI*(GLfloat)(i)/SLICES));
							vertex.push_back(radius*sin(M_PI*(GLfloat)(1)/STACKS)*sin(2*M_PI*(GLfloat)(i)/SLICES));
							vertex.push_back(radius*cos(M_PI*(GLfloat)(1)/STACKS));

							texcrd.push_back((GLfloat)i/SLICES);
							texcrd.push_back(1.0f - 1.0f/STACKS);

							vertex.push_back(radius*sin(M_PI*(GLfloat)(1)/STACKS)*cos(2*M_PI*(GLfloat)(i+1)/SLICES));
							vertex.push_back(radius*sin(M_PI*(GLfloat)(1)/STACKS)*sin(2*M_PI*(GLfloat)(i+1)/SLICES));
							vertex.push_back(radius*cos(M_PI*(GLfloat)(1)/STACKS));

							texcrd.push_back((GLfloat)i/SLICES);
							texcrd.push_back(1.0f - 1.0f/STACKS);
						}

						for(int i=0; i<SLICES; i++)
						{
							for(int j=1; j<STACKS-1; j++)
							{
								vertex.push_back(radius*sin(M_PI * (GLfloat)(j) / STACKS)*cos(2 * M_PI*(GLfloat)(i + 1) / SLICES));
								vertex.push_back(radius*sin(M_PI * (GLfloat)(j) / STACKS)*sin(2 * M_PI*(GLfloat)(i + 1) / SLICES));
								vertex.push_back(radius*cos(M_PI * (GLfloat)(j) / STACKS));

								texcrd.push_back((GLfloat)(i+1)/SLICES);
								texcrd.push_back(1.0f - (GLfloat)(j) / STACKS);

								vertex.push_back(radius*sin(M_PI * (GLfloat)(j) / STACKS)*cos(2 * M_PI*(GLfloat)(i) / SLICES));
								vertex.push_back(radius*sin(M_PI * (GLfloat)(j) / STACKS)*sin(2 * M_PI*(GLfloat)(i) / SLICES));
								vertex.push_back(radius*cos(M_PI * (GLfloat)(j) / STACKS));

								texcrd.push_back((GLfloat)(i)/SLICES);
								texcrd.push_back(1.0f - (GLfloat)(j) / STACKS);

								vertex.push_back(radius*sin(M_PI * (GLfloat)(j + 1) / STACKS)*cos(2 * M_PI*(GLfloat)(i) / SLICES));
								vertex.push_back(radius*sin(M_PI * (GLfloat)(j + 1) / STACKS)*sin(2 * M_PI*(GLfloat)(i) / SLICES));
								vertex.push_back(radius*cos(M_PI * (GLfloat)(j + 1) / STACKS));

								texcrd.push_back((GLfloat)(i)/SLICES);
								texcrd.push_back(1.0f - (GLfloat)(j+1) / STACKS);

								vertex.push_back(radius*sin(M_PI * (GLfloat)(j) / STACKS)*cos(2 * M_PI*(GLfloat)(i + 1) / SLICES));
								vertex.push_back(radius*sin(M_PI * (GLfloat)(j) / STACKS)*sin(2 * M_PI*(GLfloat)(i + 1) / SLICES));
								vertex.push_back(radius*cos(M_PI * (GLfloat)(j) / STACKS));

								texcrd.push_back((GLfloat)(i+1)/SLICES);
								texcrd.push_back(1.0f - (GLfloat)(j) / STACKS);

								vertex.push_back(radius*sin(M_PI * (GLfloat)(j + 1) / STACKS)*cos(2 * M_PI*(GLfloat)(i) / SLICES));
								vertex.push_back(radius*sin(M_PI * (GLfloat)(j + 1) / STACKS)*sin(2 * M_PI*(GLfloat)(i) / SLICES));
								vertex.push_back(radius*cos(M_PI * (GLfloat)(j + 1) / STACKS));

								texcrd.push_back((GLfloat)(i)/SLICES);
								texcrd.push_back(1.0f - (GLfloat)(j+1) / STACKS);

								vertex.push_back(radius*sin(M_PI * (GLfloat)(j + 1) / STACKS)*cos(2 * M_PI*(GLfloat)(i + 1) / SLICES));
								vertex.push_back(radius*sin(M_PI * (GLfloat)(j + 1) / STACKS)*sin(2 * M_PI*(GLfloat)(i + 1) / SLICES));
								vertex.push_back(radius*cos(M_PI * (GLfloat)(j + 1) / STACKS));

								texcrd.push_back((GLfloat)(i+1)/SLICES);
								texcrd.push_back(1.0f - (GLfloat)(j+1) / STACKS);
							}
						}

						for (int i = 0; i < SLICES; i++)
						{
							vertex.push_back(radius*sin(M_PI * (GLfloat)(STACKS - 1) / STACKS)*cos(2 * M_PI*(GLfloat)(i + 1) / SLICES));
							vertex.push_back(radius*sin(M_PI * (GLfloat)(STACKS - 1) / STACKS)*sin(2 * M_PI*(GLfloat)(i + 1) / SLICES));
							vertex.push_back(radius*cos(M_PI * (GLfloat)(STACKS - 1) / STACKS));

							texcrd.push_back((GLfloat)(i+1)/SLICES);
							texcrd.push_back(1.0f / STACKS);

							vertex.push_back(radius*sin(M_PI * (GLfloat)(STACKS - 1) / STACKS)*cos(2 * M_PI*(GLfloat)(i) / SLICES));
							vertex.push_back(radius*sin(M_PI * (GLfloat)(STACKS - 1) / STACKS)*sin(2 * M_PI*(GLfloat)(i) / SLICES));
							vertex.push_back(radius*cos(M_PI * (GLfloat)(STACKS - 1) / STACKS));

							texcrd.push_back((GLfloat)(i)/SLICES);
							texcrd.push_back(1.0f / STACKS);

							vertex.push_back(0);
							vertex.push_back(0);
							vertex.push_back(-radius);

							texcrd.push_back((GLfloat)(i)/SLICES);
							texcrd.push_back(0.0f);
						}

						for(auto && v : vertex)
						{
							normal.push_back(v/radius);
						}

					}

			};

			//Assimp Model
			class AssimpModel
			{
				AssimpModel(const AssimpModel&) = delete;
				AssimpModel(AssimpModel&&) = delete;

				private:
					Assimp::Importer importer;
					const aiScene* scene;

					AssimpModel& operator=(const AssimpModel&);
					AssimpModel& operator=(AssimpModel&&);
				public:
					AssimpModel(const std::string &filename)
						:scene(importer.ReadFile(
								filename,
								aiProcess_CalcTangentSpace			|
								aiProcess_Triangulate				|
								aiProcess_JoinIdenticalVertices	|
								aiProcess_SortByPType))
					{
						if(!scene)
						{
							//file open error
							std::cerr << "Assimp Error (Filename: " << filename << ")" << std::endl;
							std::cerr << importer.GetErrorString() << std::endl;
							return;
						}

						//DoImportScene
					}
			};
		}
	}
}
