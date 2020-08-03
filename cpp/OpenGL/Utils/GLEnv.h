#pragma once

#include <exception>
#include <string>
#include <chrono>
#include <experimental/source_location>

#include <GL/glew.h>  
#include <GLFW/glfw3.h>

class GLException : public std::exception {
	public:
		GLException(const std::string& whatStr) : whatStr(whatStr) {}
		virtual const char* what() const throw() {
			return whatStr.c_str();
		}
	private:
		std::string whatStr;
};

struct Dimensions {
	uint32_t width;
	uint32_t height;
	
	float aspect() const {return float(width)/float(height);}
};

class GLEnv {
	public:
		GLEnv(uint32_t w, uint32_t h, uint32_t s, const std::string& title, bool fpsCounter=false, bool sync=true, int major=2, int minor=1, bool core=false);
		~GLEnv();
		void setKeyCallback(GLFWkeyfun f);
        void setMouseCallbacks(GLFWcursorposfun p, GLFWmousebuttonfun b, GLFWscrollfun s);
		
		Dimensions getFramebufferSize() const;	
		bool shouldClose() const;
		void endOfFrame();
		
		void setFPSCounter(bool fpsCounter);
		void setSync(bool sync);

		static void checkGLError(const std::string& id, std::experimental::source_location where = std::experimental::source_location::current());

	private:
		GLFWwindow* window;
		std::string title;
		bool fpsCounter;
		std::chrono::high_resolution_clock::time_point last;
		uint64_t frameCount;
		
		static void errorCallback(int error, const char* description);		
};
