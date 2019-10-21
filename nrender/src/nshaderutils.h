#pragma once

// External Includes
#include <string>
#include <GL/glew.h>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>
#include "vulkan/vulkan_core.h"

namespace opengl
{
	/**
	 * All available OpenGL shader uniform types
	 * All uniform types need to have a set factory
	 * function associated with it
	 */
	enum class EGLSLType : uint8_t
	{
		Unknown = 0,			///< unknown or invalid shader uniform
		Float   = 1,			///< float
		Int     = 2,			///< int
		UInt    = 3,			///< unsigned int
		Vec2	= 4,			///< 2 float vector
		Vec3	= 5,			///< 3 float vector
		Vec4	= 6,			///< 4 float vector
		Mat2    = 7,			///< 2x2 float matrix
		Mat3    = 8,			///< 3x3 float matrix
		Mat4    = 9,			///< 4x4 float matrix
		Tex2D	= 10,			///< 2D Texture
	};

	/**
	 * Result of OpenGL shader validation
	 */
	enum class EShaderValidationResult : uint8_t
	{
		Success,				///< Shader validation succeeded
		Warning,				///< Shader validation succeeded with a warning
		Error					///< Shader validation failed
	};

	/**
	 * @return the uniform type based on the opengl uniform type
	 * @param glType: opengl internal type that describes a uniform
	 */
	EGLSLType getGLSLType(GLenum glType);

	/**
	* Represents an opengl shader attribute
	*/
	class ShaderInput
	{
	public:
		// Constructor
		ShaderInput(const std::string& name, int location, VkFormat format);
		ShaderInput() = delete;

		std::string		mName;							///< Name of the shader attribute
		int				mLocation;
		VkFormat		mFormat;

// 		/**
// 		 * @return if this shader input is an array or single value
// 		 */
// 		bool isArray() const							{ return mSize > 1; }
	};

	class ShaderUniformInput
	{
	public:
		// Constructor
		ShaderUniformInput(const std::string& name, int offset, int size, EGLSLType type);
		ShaderUniformInput() = delete;

		std::string		mName;							///< Name of the shader attribute
		int				mOffset;
		int				mSize;
		EGLSLType		mType;

		/**
		* @return if this shader input is an array or single value
		*/
		bool isArray() const { return false; }
	};

	using UniformDeclaration = ShaderUniformInput;

	class UniformBufferObjectDeclaration
	{
	public:
		UniformBufferObjectDeclaration(const std::string& name, int binding, VkShaderStageFlagBits inStage, int size) :
			mName(name),
			mBinding(binding),
			mStage(inStage),
			mSize(size)
		{
		}

		std::string											mName;
		int													mBinding;
		VkShaderStageFlagBits								mStage;
		int													mSize;
		std::vector<std::unique_ptr<UniformDeclaration>>	mDeclarations;		
	};

	// Typedefs
	using UniformDeclarations = std::unordered_map<std::string, UniformDeclaration*>;

	using ShaderVertexAttribute = ShaderInput;
	using ShaderVertexAttributes = std::unordered_map<std::string, std::unique_ptr<ShaderVertexAttribute>>;

	/**
	 * Given part of a shader (say vertex shader), validateShader will get information from OpenGl 
	 * on whether or not the shader was compiled successfully
	 * Note that the shader part must be loaded
	 * @return if the shader part is loaded correctly
	 */
	EShaderValidationResult validateShader(GLuint shader, std::string& validationMessage);


	/**
	 * Given a shader program, validateProgram will request from OpenGL, any information
	 * related to the validation or linking of the program with it's attached shaders. It will
	 * then output any issues that have occurred.
	 * Note that the shader must be loaded
	 * @return if the shader program is loaded correctly without errors
	 */
	EShaderValidationResult validateShaderProgram(GLuint program, std::string& validationMessage);


	/**
	 * Extracts all shader uniforms, note that the program must be loaded
	 * @param program: The shader program to extract uniform inputs from
	 * @param outUniforms: The populated list of uniforms
	 */
	void extractShaderUniforms(GLuint program, UniformDeclarations& outUniforms);


	/**
	 * Extracts all shader attributes, note that the program must be loaded
	 * @param program The shader program to extract attribute inputs from
	 * @param outAttributes The populated list of shader attributes
	 */
	void extractShaderAttributes(GLuint program, ShaderVertexAttributes& outAttributes);
}

//////////////////////////////////////////////////////////////////////////
// Hash
//////////////////////////////////////////////////////////////////////////
namespace std
{
	template<>
	struct hash<opengl::ShaderInput> 
	{
		size_t operator()(const opengl::ShaderInput &k) const 
		{
			return hash<std::string>()(k.mName);
		}
	};

	template <>
	struct hash<opengl::EGLSLType>
	{
		size_t operator()(const opengl::EGLSLType& v) const
		{
			return hash<uint8_t>()(static_cast<uint8_t>(v));
		}
	};
}
