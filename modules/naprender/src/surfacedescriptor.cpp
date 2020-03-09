#include "surfacedescriptor.h"
#include "rtti/typeinfo.h"

RTTI_BEGIN_CLASS(nap::SurfaceDescriptor)
	RTTI_PROPERTY("Width",		&nap::SurfaceDescriptor::mWidth,		nap::rtti::EPropertyMetaData::Default)
	RTTI_PROPERTY("Height",		&nap::SurfaceDescriptor::mHeight,		nap::rtti::EPropertyMetaData::Default)
	RTTI_PROPERTY("DataType",	&nap::SurfaceDescriptor::mDataType,		nap::rtti::EPropertyMetaData::Default)
	RTTI_PROPERTY("Channels",	&nap::SurfaceDescriptor::mChannels,		nap::rtti::EPropertyMetaData::Default)
	RTTI_PROPERTY("ColorSpace",	&nap::SurfaceDescriptor::mColorSpace,	nap::rtti::EPropertyMetaData::Default)
RTTI_END_CLASS

RTTI_BEGIN_ENUM(nap::ESurfaceChannels)
	RTTI_ENUM_VALUE(nap::ESurfaceChannels::R,		"R"),
	RTTI_ENUM_VALUE(nap::ESurfaceChannels::RGBA,	"RGBA"),
	RTTI_ENUM_VALUE(nap::ESurfaceChannels::BGRA,	"BGRA")
RTTI_END_ENUM

RTTI_BEGIN_ENUM(nap::ESurfaceDataType)
	RTTI_ENUM_VALUE(nap::ESurfaceDataType::BYTE,	"Byte"),
	RTTI_ENUM_VALUE(nap::ESurfaceDataType::USHORT,	"Short"),
	RTTI_ENUM_VALUE(nap::ESurfaceDataType::FLOAT,	"Float")
RTTI_END_ENUM

RTTI_BEGIN_ENUM(nap::EColorSpace)
	RTTI_ENUM_VALUE(nap::EColorSpace::Linear,		"Linear"),
	RTTI_ENUM_VALUE(nap::EColorSpace::sRGB,			"sRGB")
RTTI_END_ENUM


namespace nap
{
	SurfaceDescriptor::SurfaceDescriptor(uint32_t width, uint32_t height, ESurfaceDataType dataType, ESurfaceChannels channels, EColorSpace colorSpace) :
		mWidth(width),
		mHeight(height),
		mDataType(dataType),
		mChannels(channels),
		mColorSpace(colorSpace)
	{
	}

	// Returns number of components each texel has in this format
	int SurfaceDescriptor::getNumComponents() const
	{
		switch (mChannels)
		{
		case ESurfaceChannels::R:
			return 1;

		case ESurfaceChannels::RGBA:
		case ESurfaceChannels::BGRA:
			return 4;
		}

		assert(false);
		return 0;
	}

	// Returns What the size in bytes is of a component type
	int SurfaceDescriptor::getComponentSize() const
	{
		switch (mDataType)
		{
		case ESurfaceDataType::BYTE:
			return 1;
		case ESurfaceDataType::USHORT:
			return 2;
		case ESurfaceDataType::FLOAT:
			return 4;
		}

		assert(false);
		return 0;
	}

	int SurfaceDescriptor::getPitch() const
	{
		return mWidth * getBytesPerPixel();
	}

	int SurfaceDescriptor::getBytesPerPixel() const
	{
		return getComponentSize() * getNumComponents();
	}

	uint64_t SurfaceDescriptor::getSizeInBytes() const
	{
		return getPitch() * mHeight;
	}
}
