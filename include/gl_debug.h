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

#include <iostream>

namespace jikoLib{
	namespace GLLib{

#ifdef DEBUG
#define DEBUG_OUT(x) std::cout << x << std::endl;
#else
#define DEBUG_OUT(x)
#endif

		/*
		 * Check OpenGL Error
		 */

#ifdef DEBUG
		extern GLenum _err_;
#endif
#ifdef DEBUG
#define CHECK_GL_ERROR \
		_err_ = glGetError(); \
		if(_err_ == GL_INVALID_ENUM) std::cerr << "GL_INVALID_ENUM" << " at " << __FILE__ << ": " << __LINE__ << std::endl; \
		if(_err_ == GL_INVALID_VALUE) std::cerr << "GL_INVALID_VALUE" << " at " << __FILE__ << ": " << __LINE__ << std::endl; \
		if(_err_ == GL_INVALID_OPERATION) std::cerr << "GL_INVALID_OPERATION" << " at " << __FILE__ << ": " << __LINE__ << std::endl; \
		if(_err_ == GL_STACK_OVERFLOW) std::cerr << "GL_STACK_OVERFLOW" << " at " << __FILE__ << ": " << __LINE__ << std::endl; \
		if(_err_ == GL_STACK_UNDERFLOW) std::cerr << "GL_STACK_UNDERFLOW" << " at " << __FILE__ << ": " << __LINE__ << std::endl; \
		if(_err_ == GL_OUT_OF_MEMORY) std::cerr << "GL_OUT_OF_MEMORY" << " at " << __FILE__ << ": " << __LINE__ << std::endl; \
		if(_err_ == GL_TABLE_TOO_LARGE) std::cerr << "GL_TABLE_TOO_LARGE" << " at " << __FILE__ << ": " << __LINE__ << std::endl;
#else
#define CHECK_GL_ERROR
#endif
	}
}
