/*******************************************************************************
 * jikoLib
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

#include <chrono>
#include <thread>
#include <iostream>

namespace jikoLib{
	namespace AnimLib{

		//FPS fix helper
		
		/****************
		 * sample usage 
		 *
		 * FpsFix f;
		 *
		 * 
		 * while(true) //update and draw loop routine
		 * {
		 * 	f.frameStart();
		 *    //update routine1 by cpu
		 *    if(f.isDrawable())
		 * 		//draw routine1 by gpu
		 *
		 *    //update routine2 by cpu
		 *    if(f.isDrawable())
		 * 		//draw routine2 by gpu
		 *
		 *    //update routine3 by cpu
		 *    if(f.isDrawable())
		 * 		//draw routine3 by gpu
		 * 		
		 * 	...
		 *
		 *    f.frameEnd();
		 * }
		 * 
		 ***************/

		class FpsFix{
			private:
				std::chrono::microseconds set_usec;
				std::chrono::microseconds current_usec;
				bool is_drawable;
				std::chrono::high_resolution_clock::time_point startpoint;
				std::chrono::high_resolution_clock::time_point endpoint;
				int get_fps_count;
				int fps_update_interval;
				std::chrono::microseconds sum_usec;
				double current_fps;

			public:
				FpsFix(double Fps = 60.0, unsigned int fps_update_interval = 60)
					: set_usec(static_cast<long>(1000000L/Fps)),
					  current_usec(set_usec),
					  is_drawable(true),
					  get_fps_count(0),
					  fps_update_interval(fps_update_interval),
					  sum_usec(0),
					  current_fps(Fps)
				{}
				
				inline void frameStart()
				{
					//loop phase
					startpoint = std::chrono::high_resolution_clock::now();
				}

				inline bool isDrawable()
				{
					//check if drawable
					return is_drawable;
				}

				void frameEnd()
				{
					//loop end
					endpoint = std::chrono::high_resolution_clock::now();
					current_usec = std::chrono::duration_cast<std::chrono::microseconds>(endpoint - startpoint);
					
					if(current_usec < set_usec) //insert sleep
					{
						is_drawable = true;
						std::this_thread::sleep_for(set_usec - current_usec);
						endpoint = std::chrono::high_resolution_clock::now();
						current_usec = std::chrono::duration_cast<std::chrono::microseconds>(endpoint - startpoint);
					}
					else //unset drawable flag
					{
						is_drawable = false;
					}
					sum_usec += current_usec;
					get_fps_count++;
					if(get_fps_count >= fps_update_interval)
					{
						get_fps_count = 0;
						current_fps = fps_update_interval*1000000.0/sum_usec.count();
						sum_usec = std::chrono::microseconds(0);
					}
				}

				inline double getFps()
				{
					return current_fps;
				}
		};
	}
}

