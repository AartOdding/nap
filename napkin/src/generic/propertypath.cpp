#include "propertypath.h"

#include <rtti/rttiobject.h>
#include <rtti/linkresolver.h>
#include <rtti/defaultlinkresolver.h>
#include <appcontext.h>
#include <nap/logger.h>


using namespace nap;
using namespace nap::rtti;

napkin::PropertyPath::PropertyPath(RTTIObject& obj, const RTTIPath& path)
		: mObject(&obj), mPath(path)
{
}

napkin::PropertyPath::PropertyPath(RTTIObject& obj, const std::string& path)
		: mObject(&obj), mPath(RTTIPath())
{
	mPath.pushAttribute(path);
}

rttr::variant napkin::PropertyPath::getValue() const
{
	return getProperty().get_value(mObject);
}

rttr::property napkin::PropertyPath::getProperty() const
{
	return resolve().getProperty();
}

ResolvedRTTIPath napkin::PropertyPath::resolve() const
{
	ResolvedRTTIPath resolvedPath;
	mPath.resolve(mObject, resolvedPath);
	return resolvedPath;
}

RTTIObject* napkin::PropertyPath::getPointee() const
{
	auto value					   = resolve().getValue();
	auto value_type				   = value.get_type();
	auto wrapped_type			   = value_type.is_wrapper() ? value_type.get_wrapped_type() : value_type;
	bool is_wrapper				   = wrapped_type != value_type;
	RTTIObject* pointee = is_wrapper ? value.extract_wrapped_value().get_value<RTTIObject*>()
									 : value.get_value<RTTIObject*>();
	return pointee;
}

std::string napkin::PropertyPath::getPointeeID() const
{
	auto object = getPointee();
	return (object == nullptr) ? "" : object->mID;
}


bool napkin::PropertyPath::setPointee(const std::string& target_id) const
{
	nap::utility::ErrorState errorState;

	UnresolvedPointerList mUnresolvedPointers; // The list of UnresolvedPointers that was read

	if (!target_id.empty())
		mUnresolvedPointers.emplace_back(mObject, mPath, target_id);

	bool result = DefaultLinkResolver::sResolveLinks(AppContext::get().getObjects(), mUnresolvedPointers, errorState);
	if (!result)
		Logger::warn(errorState.toString());

	return result;
}

bool napkin::PropertyPath::isFileLink()
{
	return hasFlag(getProperty(), EPropertyMetaData::FileLink);
}

rttr::type napkin::PropertyPath::getArrayElementType() const
{
	auto arrayView = getArrayView();
	return arrayView.get_rank_type(arrayView.get_rank());
}

rttr::variant_array_view napkin::PropertyPath::getArrayView() const
{
	return resolve().getValue().create_array_view();
}