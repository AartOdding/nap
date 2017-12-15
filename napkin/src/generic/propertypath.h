#pragma once

#include <rtti/rttipath.h>

namespace napkin
{
	/**
	 * A path to a property, including its object.
	 */
	class PropertyPath
	{
	public:
		/**
		 * @param obj The object this property is on
		 * @param path The path to the property
		 */
		PropertyPath(nap::rtti::RTTIObject& obj, const nap::rtti::RTTIPath& path);

		/**
		 * @param obj The object this property is on
		 * @param path The path to the property
		 */
		PropertyPath(nap::rtti::RTTIObject& obj, const std::string& path);

		/**
		 * @return The value of this property
		 */
		rttr::variant getValue() const;

		/**
		 * @return The property this path points to
		 */
		rttr::property getProperty() const;

		/**
		 * @return obj The object this property is on
		 */
		nap::rtti::RTTIObject& object() const { return *mObject; }

		/**
		 * @return path The path to the property
		 */
		const nap::rtti::RTTIPath& path() const { return mPath; }

		/**
		 * Resolve a property path
		 */
		nap::rtti::ResolvedRTTIPath resolve() const;

		/**
		 * Given a Pointer Property (or how do you call them), find the object it's pointing to.
		 */
		nap::rtti::RTTIObject* getPointee() const;

		/**
		 * Given a Pointer Property (i like this name), set its pointee using a string.
		 */
		bool setPointee(const std::string& target) const;

		/**
		 * Get the identifier of the pointee, only if this path is a Pointer Property
		 */
		std::string getPointeeID() const;

		/**
		 * If this refers to an array property, get the element type
		 */
		rttr::type getArrayElementType() const;

		/**
		 * If this property is an array, get its arrayview
		 */
		rttr::variant_array_view getArrayView() const;

		/**
		 * If this property path represents a file link
		 */
		bool isFileLink();


	private:
		nap::rtti::RTTIObject* mObject;
		nap::rtti::RTTIPath mPath;
	};
}