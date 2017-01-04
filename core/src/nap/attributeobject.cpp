// Local includes
#include "attributeobject.h"
#include "component.h"
#include "rttinap.h"

// External includes
#include <algorithm>


RTTI_DEFINE(nap::AttributeObject)

namespace nap {
    
    AttributeObject::AttributeObject(Object* parent, const std::string& name)
    {
        mName = name;
        parent->addChild(*this);
    }
    

    AttributeBase& AttributeObject::addAttribute(const std::string& name, RTTI::TypeInfo type)
    {
        assert(type.isKindOf<AttributeBase>());
        return *static_cast<AttributeBase*>(&addChild(name, type));
    }

    
    bool AttributeObject::hasAttribute(const std::string& name) const
    {
        return hasChildOfType<AttributeBase>(name);
    }

	AttributeBase *AttributeObject::getOrCreateAttribute(const std::string &name, const RTTI::TypeInfo &valueType) {
		auto attribute = getAttribute(name);

		if (attribute) {
			if (attribute->getValueType() != valueType) {
				Logger::warn("Attribute of type '%s' existed, but had different type: %s", attribute->getValueType().getName().c_str(), valueType.getName().c_str());
				return nullptr;
			}
		}else{
			attribute = &addAttribute(name, getAttributeTypeFromValueType(valueType));
		}
		return attribute;
	}

}
