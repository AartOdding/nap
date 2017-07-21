#include "objectptr.h"

namespace nap
{
	ObjectPtrManager& ObjectPtrManager::get()
	{
		static ObjectPtrManager manager;
		return manager;
	}
}