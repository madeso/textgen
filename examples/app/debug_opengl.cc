#include "debug_opengl.h"

#include "glad/glad.h"
#include "SDL.h"

#include <string>
#include <sstream>
#include <cassert>

const char* string_from_opengl_error_enum(GLenum error_code)
{
    switch(error_code)
    {
	case GL_INVALID_ENUM: return "INVALID_ENUM";
	case GL_INVALID_VALUE: return "INVALID_VALUE";
	case GL_INVALID_OPERATION: return "INVALID_OPERATION";
#ifdef GL_STACK_OVERFLOW
	case GL_STACK_OVERFLOW: return "STACK_OVERFLOW";
#endif
#ifdef GL_STACK_UNDERFLOW
	case GL_STACK_UNDERFLOW: return "STACK_UNDERFLOW";
#endif
	case GL_OUT_OF_MEMORY: return "OUT_OF_MEMORY";
	case GL_INVALID_FRAMEBUFFER_OPERATION: return "INVALID_FRAMEBUFFER_OPERATION";
	default: return "UNKNOWN";
    }
}


namespace
{
	const char* string_from_debug_source(GLenum source)
    {
        switch(source)
        {
		case GL_DEBUG_SOURCE_API: return "API";
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "Window System";
		case GL_DEBUG_SOURCE_SHADER_COMPILER: return "Shader Compiler";
		case GL_DEBUG_SOURCE_THIRD_PARTY: return "Third Party";
		case GL_DEBUG_SOURCE_APPLICATION: return "Application";
		case GL_DEBUG_SOURCE_OTHER: return "Other";
        default: return "Unknown";
        }
    }

	const char* string_from_debug_type(GLenum type)
    {
        switch(type)
        {
		case GL_DEBUG_TYPE_ERROR: return "Error";
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "Deprecated Behaviour";
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "Undefined Behaviour";
		case GL_DEBUG_TYPE_PORTABILITY: return "Portability";
		case GL_DEBUG_TYPE_PERFORMANCE: return "Performance";
		case GL_DEBUG_TYPE_OTHER: return "Other";
        default: return "Unknown";
        }
    }

	const char* string_from_debug_severity(GLenum severity)
    {
        switch(severity)
        {
		case GL_DEBUG_SEVERITY_HIGH: return "High";
		case GL_DEBUG_SEVERITY_MEDIUM: return "Medium";
		case GL_DEBUG_SEVERITY_LOW: return "Low";
		case GL_DEBUG_SEVERITY_NOTIFICATION: return "Notification";
		default: return "Unknown";
        }
    }

}  // namespace

void APIENTRY on_opengl_debug_output(
        GLenum source,
        GLenum type,
        GLuint id,
        GLenum severity,
        GLsizei /*length*/,
        const GLchar* message,
        const GLvoid* /*userParam*/
)
{
	switch (type)
	{
	// this is from ScopedDebugGroup
	case GL_DEBUG_TYPE_PUSH_GROUP:
	case GL_DEBUG_TYPE_POP_GROUP: return;
	default: break;
	}

    // ignore non-significant error/warning codes
	const auto is_important = type != GL_DEBUG_TYPE_OTHER;
	const bool is_low = severity == GL_DEBUG_SEVERITY_LOW || severity == GL_DEBUG_SEVERITY_NOTIFICATION;

	if (is_important == false && is_low)
	{
		/*
		Tries to hide the following notification:
		 Buffer object 40 (bound to GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING_ARB (0),
		 	GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING_ARB (1), GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING_ARB (2),
			and GL_ARRAY_BUFFER_ARB, usage hint is GL_STREAM_DRAW) will use VIDEO memory as the source
			for buffer object operations.
		 Buffer object 41 (bound to GL_ELEMENT_ARRAY_BUFFER_ARB, usage hint is
		 	GL_STREAM_DRAW) will use VIDEO memory as the source for buffer object operations.
		 */
		return;
	}

	// only display the first 10 notifications
	static int error_count = 0;
	if (is_important == false)
	{
		if (error_count > 10)
		{
			return;
		}
		++error_count;
	}

	std::ostringstream ss;
	ss
		<< "OpenGL #" << id
		 << " ["
			"src: "
		 << string_from_debug_source(source)
		 << " | "
			"type: "
		 << string_from_debug_type(type)
		 << " | "
			"sev: "
		 << string_from_debug_severity(severity)
		 << "] "
			": "
		 << message;
	const std::string to_out = ss.str();

	if (is_low)
	{
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "%s", to_out.c_str());
	}
	else
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "%s", to_out.c_str());
	}

	if (is_important)
	{
		assert(false && "OpenGL error");
	}
}

bool has_khr_debug()
{
	return GLAD_GL_KHR_debug != 0;
}

void setup_opengl_debug()
{
	if (has_khr_debug())
	{
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Enabling OpenGL debug output (KHR_debug)");
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_KHR);
		glDebugMessageCallback(on_opengl_debug_output, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}
	else
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "KHR_debug not available, OpenGL debug output not enabled");
	}
}

