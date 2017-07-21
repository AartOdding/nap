#pragma once

// Local includes
#include "nbitmap.h"

// External includes
#include <string>
#include <GL/glew.h>
#include <unordered_map>
#include <FreeImage.h>

namespace opengl
{
	/**
	 * getBitmapForFreeImageType
	 *
	 * Returns the associated Bitmap Data Type for the queried free image type
	 * @return: the associated bitmap type, UNKNOWN if not found
	 */
	BitmapDataType	getBitmapType(FREE_IMAGE_TYPE type);

	/**
	 * getBitmapColor
	 *
	 * Returns the associated Bitmap Color Type for the queried free image color type
	 * @return: the associated bitmap color, UNKNOWN if not found
	 */
	BitmapColorType getColorType(FREE_IMAGE_COLOR_TYPE colorType, FREE_IMAGE_TYPE dataType);

	/**
	 * loadBitmap
	 *
	 * Creates a bitmap from file, this bitmap owns the pixel data
	 * @return: a new bitmap, nullptr if unsuccessful
	 * @param imgPath: full path to image to load
	 */
	Bitmap*			loadBitmap(const std::string& imgPath);

	/**
	 * loadBitmap
	 *
	 * loads a bitmap from file, the bitmap owns the pixel data
	 * if the bitmap already contains data that data will be cleared!
	 * @param bitmap: the bitmap to set, clears existing data!
	 * @param imgPath: full path to image to load
	 * @return if the load was successful or not
	 */
	bool			loadBitmap(Bitmap& bitmap, const std::string& imgPath);
}


namespace std
{
    template<>
    struct hash<FREE_IMAGE_COLOR_TYPE>
    {
        size_t operator()(const FREE_IMAGE_COLOR_TYPE &v) const
        {
            return hash<int>()(static_cast<int>(v));
        }
    };
}