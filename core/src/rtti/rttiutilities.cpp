#include <rtti/rttiutilities.h>
#include <nap/object.h>

namespace RTTI
{
	/**
	 * Helper function to recursively visit the properties of an object (without following pointers).
	 *
	 * A functor-like parameter can be provided that will be invoked for each property visited. The signature of the parameter should be as follows:
	 *		void visitFunction(const RTTI::Instance& instance, const RTTI::Property& property, const RTTI::Variant& value, const RTTI::RTTIPath& path)
	 *
	 * The parameters of the visit function are as follows:
	 *  - instance: the instance (object) we're visiting
	 *  - property: the property on the object we're visiting
	 *  - value: the value of the property we're visiting
	 *  - path: full RTTIPath to the property we're visiting
	 *
	 */
	template<class FUNC>
	void VisitRTTIPropertiesRecursive(const Variant& variant, RTTIPath& path, FUNC& visitFunc)
	{
		// Extract wrapped type
		auto value_type = variant.get_type();
		auto actual_type = value_type.is_wrapper() ? value_type.get_wrapped_type() : value_type;

		// If this is an array, recurse into the array for each element
		if (actual_type.is_array())
		{
			VariantArray array = variant.create_array_view();

			// Recursively visit each array element
			for (int index = 0; index < array.get_size(); ++index)
			{
				path.PushArrayElement(index);

				RTTI::Variant array_value = array.get_value_as_ref(index);

				// Recurse
				VisitRTTIPropertiesRecursive(array_value, path, visitFunc);

				path.PopBack();
			}
		}
		else if (!actual_type.is_pointer()) // Don't recurse into properties of pointers
		{
			// Recursively visit each property of the type
			for (const RTTI::Property& property : actual_type.get_properties())
			{
				path.PushAttribute(property.get_name().data());

				RTTI::Variant value = property.get_value(variant);
				
				// Invoke visit func
				visitFunc(variant, property, value, path);

				// Recurse
				VisitRTTIPropertiesRecursive(value, path, visitFunc);

				path.PopBack();
			}
		}
	}

	/**
	 * Helper function to recursively visit the properties of an object (without following pointers).
	 *
	 * A functor-like parameter can be provided that will be invoked for each property visited. The signature of the parameter should be as follows:
	 *		void visitFunction(const RTTI::Instance& instance, const RTTI::Property& property, const RTTI::Variant& value, const RTTI::RTTIPath& path)
	 *
	 * The parameters of the visit function are as follows:
	 *  - instance: the instance (object) we're visiting
	 *  - property: the property on the object we're visiting
	 *  - value: the value of the property we're visiting
	 *  - path: full RTTIPath to the property we're visiting
	 *
	 */
	template<class FUNC>
	void VisitRTTIProperties(const Instance& instance, RTTIPath& path, FUNC& visitFunc)
	{
		// Recursively visit each property of the type
		for (const RTTI::Property& property : instance.get_derived_type().get_properties())
		{
			path.PushAttribute(property.get_name().data());

			RTTI::Variant value = property.get_value(instance);

			// Invoke visit func
			visitFunc(instance, property, value, path);

			// Recurse
			VisitRTTIPropertiesRecursive(value, path, visitFunc);

			path.PopBack();
		}
	}


	/**
	 * ObjectLinkVisitor is a functor-like object that can be used as an argument to VisitRTTIProperties and collects all links to other nap::Objects 
	 */
	struct ObjectLinkVisitor
	{
	public:
		ObjectLinkVisitor(const nap::Object& sourceObject, std::vector<ObjectLink>& objectLinks) :
			mSourceObject(sourceObject),
			mObjectLinks(objectLinks)
		{
		}

		void operator()(const Instance& instance, const Property& property, const Variant& value, const RTTIPath& path)
		{
			if (!property.get_type().is_pointer())
				return;

			assert(value.get_type().is_derived_from<nap::Object>());
			mObjectLinks.push_back({ &mSourceObject, path, value.convert<nap::Object*>() });
		}

	private:
		const nap::Object&			mSourceObject;	// The object we're visiting (i.e. the source of any link found)
		std::vector<ObjectLink>&	mObjectLinks;	// Array of all links found
	};


	/**
	 * FileLinkVisitor is a functor-like object that can be used as an argument to VisitRTTIProperties and collects all file links
	 */
	struct FileLinkVisitor
	{
	public:
		FileLinkVisitor(std::vector<std::string>& fileLinks) :
			mFileLinks(fileLinks)
		{
		}

		void operator()(const Instance& instance, const Property& property, const Variant& value, const RTTIPath& path)
		{
			if (!property.get_metadata(RTTI::EPropertyMetaData::FileLink).is_valid())
				return;

			assert(value.get_type().is_derived_from<std::string>());
			mFileLinks.push_back(value.convert<std::string>());
		}

	private:
		std::vector<std::string>&	mFileLinks;	// Array of file links found
	};


	/**
	 * Helper function to recursively check whether two variants (i.e. values) are equal
	 * Correctly deals with arrays and nested compounds, but note: does not follow pointers
	 */
	bool areVariantsEqualRecursive(const RTTI::Variant& variantA, const RTTI::Variant& variantB, EPointerComparisonMode pointerComparisonMode)
	{
		// Extract wrapped type
		auto value_type = variantA.get_type();
		auto actual_type = value_type.is_wrapper() ? value_type.get_wrapped_type() : value_type;
		bool is_wrapper = actual_type != value_type;
		
		// Types must match
		assert(value_type == variantB.get_type());

		// If this is an array, compare the array element-wise
		if (value_type.is_array())
		{
			// Get the arrays
			RTTI::VariantArray array_a = variantA.create_array_view();
			RTTI::VariantArray array_b = variantB.create_array_view();

			// If the sizes don't match, the arrays can't be equal
			if (array_a.get_size() != array_b.get_size())
				return false;

			// Recursively compare each array element
			for (int index = 0; index < array_a.get_size(); ++index)
			{
				RTTI::Variant array_value_a = array_a.get_value_as_ref(index);
				RTTI::Variant array_value_b = array_a.get_value_as_ref(index);

				if (!areVariantsEqualRecursive(array_value_a, array_value_b, pointerComparisonMode))
					return false;
			}
		}
		else
		{
			// Special case handling for pointers so we can compare by ID or by actual pointer value
			if (value_type.is_pointer())
			{
				// If we don't want to compare by ID, just check the pointers directly
				if (pointerComparisonMode == EPointerComparisonMode::BY_POINTER)
				{
					return is_wrapper ? (variantA.extract_wrapped_value() == variantB.extract_wrapped_value()) : (variantA == variantB);
				}
				else if (pointerComparisonMode == EPointerComparisonMode::BY_ID)
				{
					// Extract the pointer
					RTTI::Variant value_a = is_wrapper ? variantA.extract_wrapped_value() : variantA;
					RTTI::Variant value_b = is_wrapper ? variantB.extract_wrapped_value() : variantB;

					// Can only compare pointers that are of type Object
					assert(value_a.get_type().is_derived_from<nap::Object>() && value_b.get_type().is_derived_from<nap::Object>());

					// Extract the objects
					nap::Object* object_a = value_a.convert<nap::Object*>();
					nap::Object* object_b = value_b.convert<nap::Object*>();

					// If both are null, they're equal
					if (object_a == nullptr && object_b == nullptr)
						return true;

					// If only one is null, they can't be equal
					if (object_a == nullptr || object_b == nullptr)
						return false;

					// Check whether the IDs match
					return object_a->mID == object_b->mID;
				}
				else
				{
					assert(false);
				}
			}

			// If the type of this variant is a primitive type or non-primitive type with no RTTI properties,
			// we perform a normal comparison
			auto child_properties = actual_type.get_properties();
			if (RTTI::isPrimitive(value_type) || child_properties.empty())
				return is_wrapper ? (variantA.extract_wrapped_value() == variantB.extract_wrapped_value()) : (variantA == variantB);

			// Recursively compare each property of the compound
			for (const RTTI::Property& property : child_properties)
			{
				RTTI::Variant value_a = property.get_value(variantA);
				RTTI::Variant value_b = property.get_value(variantB);
				if (!areVariantsEqualRecursive(value_a, value_b, pointerComparisonMode))
					return false;
			}
		}

		return true;
	}


	/**
	* Copies rtti attributes from one object to another.
	*/
	void copyObject(const nap::Object& srcObject, nap::Object& dstObject)
	{
		RTTI::TypeInfo type = srcObject.get_type();
		assert(type == dstObject.get_type());

		for (const RTTI::Property& property : type.get_properties())
		{
			RTTI::Variant new_value = property.get_value(srcObject);
			property.set_value(dstObject, new_value);
		}
	}

	
	/**
	* Tests whether the attributes of two objects have the same values.
	* @param objectA: first object to compare attributes from.
	* @param objectB: second object to compare attributes from.
	*/
	bool areObjectsEqual(const nap::Object& objectA, const nap::Object& objectB, EPointerComparisonMode pointerComparisonMode)
	{
		RTTI::TypeInfo typeA = objectA.get_type();
		assert(typeA == objectB.get_type());

		for (const RTTI::Property& property : typeA.get_properties())
		{
			RTTI::Variant valueA = property.get_value(objectA);
			RTTI::Variant valueB = property.get_value(objectB);
			if (!areVariantsEqualRecursive(valueA, valueB, pointerComparisonMode))
				return false;
		}

		return true;
	}


	/**
	* Searches through object's rtti attributes for attribute that have the 'file link' tag.
	*/
	void findFileLinks(const nap::Object& object, std::vector<std::string>& fileLinks)
	{
		fileLinks.clear();

		RTTIPath path;
		FileLinkVisitor visitor(fileLinks);
		VisitRTTIProperties(object, path, visitor);
	}
	
	
	/**
	* Searches through object's rtti attributes for pointer attributes.
	*/
	void findObjectLinks(const nap::Object& object, std::vector<ObjectLink>& objectLinks)
	{
		objectLinks.clear();

		RTTIPath path;
		ObjectLinkVisitor visitor(object, objectLinks);
		VisitRTTIProperties(object, path, visitor);
	}


	/**
	 * Helper function to recursively build a type version string for a given RTTI type
	 */
	void appendTypeInfoToVersionStringRecursive(const RTTI::TypeInfo& type, std::string& versionString)
	{
		// Append name of type
		versionString.append(type.get_name().data(), type.get_name().size());

		// Append properties
		for (const RTTI::Property& property : type.get_properties())
		{
			// Append property name + type
			versionString.append(property.get_name().data(), property.get_name().size());
			versionString.append(property.get_type().get_name().data(), property.get_type().get_name().size());
			
			RTTI::TypeInfo type = property.get_type();

			// TODO: array/map support
// 			if (type.is_array())
// 			{
// 				if (type.can_create_instance())
// 				{
// 					RTTI::Variant array_inst = type.create();
// 					RTTI::VariantArray array_view = array_inst.create_array_view();
// 					type = array_view.get_rank_type(array_view.get_rank());
// 				}
// 			}				

			// Don't recurse into primitives, pointers or types without further properties
			if (RTTI::isPrimitive(type) || type.is_pointer() || type.get_properties().empty())
				continue;

			// Recurse
			appendTypeInfoToVersionStringRecursive(type, versionString);
		}
	}


	/**
	 * Calculate the version number of the specified type
	 */
	std::size_t getRTTIVersion(const RTTI::TypeInfo& type)
	{
		// Build the version string first
		std::string version_string;
		appendTypeInfoToVersionStringRecursive(type, version_string);

		// Hash
		std::size_t hash = std::hash<std::string>()(version_string);
		return hash;
	}
}
