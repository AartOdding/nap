#pragma once

// External Includes
#include <parameter.h>
#include <nap/signalslot.h>
#include <color.h>

namespace nap
{
	/**
	 * Parameter that simply wraps a value without any further metadata
	 */
	template<typename T>
	class ParameterSimple : public Parameter
	{
		RTTI_ENABLE(Parameter)
	public:

		/**
		 * Set the value of this enum from another parameter
		 *
		 * @param value The parameter to set the value from
		 */
		virtual void setValue(const Parameter& value) override
		{
			const ParameterSimple<T>* derived_type = rtti_cast<const ParameterSimple<T>>(&value);
			assert(derived_type != nullptr);
			setValue(derived_type->mValue);
		}

		/**
		 * Set the value of this parameter. Will raise the valueChanged signal if the value actually changes.
		 *
		 * @param value The value to set
		 */
		void setValue(const T& value)
		{
			if (value != mValue)
			{
				mValue = value;
				valueChanged(mValue);
			}
		}

	public:
		T			mValue;				///< Property: 'Value' the value of this parameter
		Signal<T>	valueChanged;		///< Signal that's raised when the value of this parameter changes
	};

	//////////////////////////////////////////////////////////////////////////
	// Parameter Type Definitions
	//////////////////////////////////////////////////////////////////////////

	using ParameterRGBColorFloat	= ParameterSimple<RGBColorFloat>;
	using ParameterBool				= ParameterSimple<bool>;
}
