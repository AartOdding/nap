#include "texture2d.h"

RTTI_BEGIN_ENUM(nap::Texture2D::EFormat)
	RTTI_ENUM_VALUE(nap::Texture2D::EFormat::RGBA8,	"RGBA8"),
	RTTI_ENUM_VALUE(nap::Texture2D::EFormat::RGB8,	"RGB8"),
	RTTI_ENUM_VALUE(nap::Texture2D::EFormat::R8,	"R8"),	
	RTTI_ENUM_VALUE(nap::Texture2D::EFormat::Depth,	"Depth")
RTTI_END_ENUM

RTTI_BEGIN_CLASS(nap::Texture2D)
	RTTI_PROPERTY("Width",	&nap::Texture2D::mWidth, nap::rtti::EPropertyMetaData::Required)
	RTTI_PROPERTY("Height", &nap::Texture2D::mHeight, nap::rtti::EPropertyMetaData::Required)
	RTTI_PROPERTY("Format", &nap::Texture2D::mFormat, nap::rtti::EPropertyMetaData::Required)
RTTI_END_CLASS


namespace nap
{
	// Initializes 2D texture. 
	bool Texture2D::init(utility::ErrorState& errorState)
	{
		opengl::Texture2DSettings settings;
		settings.mWidth = mWidth;
		settings.mHeight = mHeight;

		switch (mFormat)
	{
		case EFormat::RGBA8:
			settings.mFormat			= GL_RGBA;
			settings.mInternalFormat 	= GL_RGBA8;
			settings.mType				= GL_UNSIGNED_BYTE;
			break;
		case EFormat::RGB8:
			settings.mFormat			= GL_RGB;
			settings.mInternalFormat 	= GL_RGB8;
			settings.mType				= GL_UNSIGNED_BYTE;
			break;
		case EFormat::R8:
			settings.mFormat			= GL_RED;
			settings.mInternalFormat 	= GL_R8;
			settings.mType				= GL_UNSIGNED_BYTE;
			break;
		case EFormat::Depth:
			settings.mFormat			= GL_DEPTH_COMPONENT;
			settings.mInternalFormat 	= GL_DEPTH_COMPONENT;
			settings.mType				= GL_FLOAT;
			break;
	}

		BaseTexture2D::init(settings);
		return true;
	}

}
