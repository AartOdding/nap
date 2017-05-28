#include "uniforms.h"
#include "nglutils.h"
#include "imageresource.h"


RTTI_BEGIN_BASE_CLASS(nap::Uniform)
	RTTI_PROPERTY("Name", &nap::Uniform::mName, nap::rtti::EPropertyMetaData::Required)
RTTI_END_CLASS

RTTI_BEGIN_BASE_CLASS(nap::UniformValue)
RTTI_END_CLASS

RTTI_BEGIN_BASE_CLASS(nap::UniformTexture)
RTTI_END_CLASS

RTTI_BEGIN_CLASS(nap::UniformInt)
	RTTI_PROPERTY("Value", &nap::UniformInt::mValue, nap::rtti::EPropertyMetaData::Required)
RTTI_END_CLASS

RTTI_BEGIN_CLASS(nap::UniformVec4)
	RTTI_PROPERTY("Value", &nap::UniformVec4::mValue, nap::rtti::EPropertyMetaData::Required)
RTTI_END_CLASS

RTTI_BEGIN_CLASS(nap::UniformMat4)
	RTTI_PROPERTY("Value", &nap::UniformMat4::mValue, nap::rtti::EPropertyMetaData::Required)
RTTI_END_CLASS

RTTI_BEGIN_CLASS(nap::UniformTexture2D)
	RTTI_PROPERTY("Texture", &nap::UniformTexture2D::mTexture, nap::rtti::EPropertyMetaData::Required)
RTTI_END_CLASS


namespace nap
{
	void UniformInt::push(const opengl::UniformDeclaration& declaration) const
	{
		glUniform1iv(declaration.mLocation, declaration.mSize, static_cast<const GLint*>(&mValue));
		glAssert();
	}


	void UniformVec4::push(const opengl::UniformDeclaration& declaration) const
	{
		glUniform4fv(declaration.mLocation, declaration.mSize, static_cast<const GLfloat*>(&mValue.x));
		glAssert();
	}


	void UniformMat4::push(const opengl::UniformDeclaration& declaration) const
	{
		glUniformMatrix4fv(declaration.mLocation, declaration.mSize, GL_FALSE, static_cast<const GLfloat*>(&mValue[0].x));
		glAssert();
	}


	void UniformTexture2D::push(const opengl::UniformDeclaration& declaration, int textureUnit) const
	{
		if (!mTexture)
			return;

		glActiveTexture(GL_TEXTURE0 + textureUnit);

		mTexture->bind();

		glUniform1iv(declaration.mLocation, declaration.mSize, static_cast<const GLint*>(&textureUnit));
	}

} // End Namespace NAP