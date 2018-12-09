// Local Includes
#include "nglutils.h"

// External Includes
#include <unordered_set>
#include <assert.h>

namespace opengl
{
	// Simple opengl gl disable / enable function
	static void enableGLParam(GLenum param, bool enable)
	{
		if (enable)
		{
			glEnable(param);
			return;
		}
		glDisable(param);
	}


	// Clears the back buffer bit of the currently active context
	void clear(GLuint bit)
	{
		glClear(bit);
	}


	// Clears color buffer of the currently active context
	void clearColor(float r, float g, float b, float a)
	{
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glClearColor(r, g, b, a);
		opengl::clear(GL_COLOR_BUFFER_BIT);
	}


	// Clears the depth buffer of the currently active context
	void clearDepth()
	{
		glDepthMask(GL_TRUE);
		opengl::clear(GL_DEPTH_BUFFER_BIT);
	}


	// Clears the stencil buffer of the currently active context
	void clearStencil()
	{
		glStencilMask(1);
		opengl::clear(GL_STENCIL_BUFFER_BIT);
	}


	// Clears the accumulated buffer of the currently active context
	void clearAccumulated()
	{
		opengl::clear(GL_ACCUM_BUFFER_BIT);
	}


	// Disable / Enable depth test
	void enableDepthTest(bool value)
	{
		enableGLParam(GL_DEPTH_TEST, value);
	}


	bool isDepthTestEnabled()
	{
		return glIsEnabled(GL_DEPTH_TEST) == GL_TRUE;
	}


	//force execution of GL commands in finite time
	void flush()
	{
		glFlush();
	}


	// Checks if the specified filter supports mip mapping
	bool isMipMap(GLint filterType)
	{
		static std::unordered_set<GLint> mipMapMinFilterTypes;
		if (mipMapMinFilterTypes.empty())
		{
			mipMapMinFilterTypes.emplace(GL_NEAREST_MIPMAP_NEAREST);
			mipMapMinFilterTypes.emplace(GL_LINEAR_MIPMAP_NEAREST);
			mipMapMinFilterTypes.emplace(GL_NEAREST_MIPMAP_LINEAR);
			mipMapMinFilterTypes.emplace(GL_LINEAR_MIPMAP_LINEAR);
		}
		return mipMapMinFilterTypes.find(filterType) != mipMapMinFilterTypes.end();
	}


	// Turns alpha blending on / off
	void enableBlending(bool value)
	{
		enableGLParam(GL_BLEND, value);
	}


	bool isBlendingEnabled()
	{
		return glIsEnabled(GL_BLEND) == GL_TRUE;
	}


	void enableScissorTest(bool value)
	{
		enableGLParam(GL_SCISSOR_TEST, value);
	}


	bool isScissorTestEnabled()
	{
		return glIsEnabled(GL_SCISSOR_TEST) == GL_TRUE;
	}


	void enableFaceCulling(bool value)
	{
		enableGLParam(GL_CULL_FACE, value);
	}


	bool isFaceCullingEnabled()
	{
		return glIsEnabled(GL_CULL_FACE) == GL_TRUE;
	}


	// Turns multi sampling on / off
	void enableMultiSampling(bool value)
	{
		enableGLParam(GL_MULTISAMPLE_ARB, value);
	}


	bool isMultisamplingEnabled()
	{
		return glIsEnabled(GL_MULTISAMPLE_ARB) == GL_TRUE;
	}


	// Set the render viewport
	void setViewport(int width, int height)
	{
		glViewport(0, 0, width, height);
	}


	// If line smoothing should be turned on or off
	void enableLineSmoothing(bool value)
	{
		if (value)
		{
			glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
			glEnable(GL_LINE_SMOOTH);
			return;
		}
		glDisable(GL_LINE_SMOOTH);
	}


	// Set line width of rasterized lines
	void setLineWidth(float value)
	{
		glLineWidth(value);
	}


	float getLineWidth()
	{
		float v;
		glGetFloatv(GL_LINE_WIDTH, &v);
		return v;
	}

	// Mode to use when drawing polygons
	void setPolygonMode(EPolygonMode mode)
	{
		switch (mode)
		{
		case EPolygonMode::Fill:
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			break;
		case EPolygonMode::Line:
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			break;
		case EPolygonMode::Point:
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
			break;
		default:
			assert(false);
		}
	}


	// Set render point size
	void setPointSize(float size)
	{
		glPointSize(size);
	}


	// Turn smoothing of points on or off
	void enablePointSmoothing(bool value)
	{
		if (value)
		{
			glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
			glEnable(GL_POINT_SMOOTH);
			return;
		}
		glDisable(GL_POINT_SMOOTH);
	}


	float getDepth(int x, int y)
	{
		float v;
		glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &v);
		return v;
	}


	void getColor(int x, int y, std::array<std::uint8_t, 3>& color)
	{
		glReadPixels(x, y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, (void*)(&color[0]));
	}


	bool printErrorMessage(char *file, int line)
	{
		GLenum error_code;

		// Get error
		error_code = glGetError();
		if (error_code != GL_NO_ERROR)
		{
			printMessage(EGLSLMessageType::Error, "file: %s, line: %d, %s (OpenGL error code: 0x%0x)", file, line, glewGetErrorString(error_code), error_code);
#ifdef __APPLE__
            return false;
#else
            return true;
#endif
		}
		return false;
	}
}
