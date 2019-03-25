#include "parametergui.h"
#include "parameter.h"
#include "parameterservice.h"
#include "imgui/imgui.h"
#include "rtti/rttiutilities.h"
#include "parameternumeric.h"
#include "parametersimple.h"
#include "parameterenum.h"

namespace nap
{
	template<class PARAMETERTYPE>
	static void showFloatParameter(PARAMETERTYPE& parameter)
	{
		float value = parameter.mValue;
		if (ImGui::SliderFloat(parameter.mID.c_str(), &value, parameter.mMinimum, parameter.mMaximum))
			parameter.setValue(value);
	}

	template<class PARAMETERTYPE>
	static void showIntParameter(PARAMETERTYPE& parameter)
	{
		int value = parameter.mValue;
		if (ImGui::SliderInt(parameter.mID.c_str(), &value, parameter.mMinimum, parameter.mMaximum))
			parameter.setValue(value);
	}

	//////////////////////////////////////////////////////////////////////////

	ParameterGUI::ParameterGUI(ParameterService& parameterService) :
		mParameterService(parameterService)
	{
		mPresets = mParameterService.getPresets();
		
		registerDefaultParameterEditors();
	}

	void ParameterGUI::registerDefaultParameterEditors()
	{
		registerParameterEditor(RTTI_OF(ParameterFloat), [](Parameter& parameter) 
		{
			ParameterFloat* float_parameter = rtti_cast<ParameterFloat>(&parameter);
			showFloatParameter(*float_parameter);
		});

		registerParameterEditor(RTTI_OF(ParameterDouble), [](Parameter& parameter)
		{
			ParameterDouble* double_parameter = rtti_cast<ParameterDouble>(&parameter);
			showFloatParameter(*double_parameter);
		});

		registerParameterEditor(RTTI_OF(ParameterInt), [](Parameter& parameter)
		{
			ParameterInt* int_parameter = rtti_cast<ParameterInt>(&parameter);
			showIntParameter(*int_parameter);
		});

		registerParameterEditor(RTTI_OF(ParameterLong), [](Parameter& parameter)
		{
			ParameterLong* long_parameter = rtti_cast<ParameterLong>(&parameter);
			showIntParameter(*long_parameter);
		});

		registerParameterEditor(RTTI_OF(ParameterByte), [](Parameter& parameter)
		{
			ParameterByte* byte_parameter = rtti_cast<ParameterByte>(&parameter);
			showIntParameter(*byte_parameter);
		});

		registerParameterEditor(RTTI_OF(ParameterChar), [](Parameter& parameter)
		{
			ParameterChar* char_parameter = rtti_cast<ParameterChar>(&parameter);
			showIntParameter(*char_parameter);
		});

		registerParameterEditor(RTTI_OF(ParameterBool), [](Parameter& parameter)
		{
			ParameterBool* bool_parameter = rtti_cast<ParameterBool>(&parameter);

			bool value = bool_parameter->mValue;
			if (ImGui::Checkbox(bool_parameter->mID.c_str(), &value))
				bool_parameter->setValue(value);
		});

		registerParameterEditor(RTTI_OF(ParameterRGBColorFloat), [](Parameter& parameter)
		{
			ParameterRGBColorFloat* color_parameter = rtti_cast<ParameterRGBColorFloat>(&parameter);

			RGBColorFloat value = color_parameter->mValue;
			if (ImGui::ColorEdit3(color_parameter->mID.c_str(), value.getData()))
				color_parameter->setValue(value);
		});

		registerParameterEditor(RTTI_OF(ParameterVec2), [](Parameter& parameter)
		{
			ParameterVec2* vec2_parameter = rtti_cast<ParameterVec2>(&parameter);

			glm::vec2 value = vec2_parameter->mValue;
			if (ImGui::SliderFloat2(vec2_parameter->mID.c_str(), &(value[0]), vec2_parameter->mMinimum, vec2_parameter->mMaximum))
				vec2_parameter->setValue(value);
		});

		registerParameterEditor(RTTI_OF(ParameterVec3), [](Parameter& parameter)
		{
			ParameterVec3* vec3_parameter = rtti_cast<ParameterVec3>(&parameter);

			glm::vec3 value = vec3_parameter->mValue;
			if (ImGui::SliderFloat3(vec3_parameter->mID.c_str(), &(value[0]), vec3_parameter->mMinimum, vec3_parameter->mMaximum))
				vec3_parameter->setValue(value);
		});

		registerParameterEditor(RTTI_OF(ParameterEnumBase), [](Parameter& parameter)
		{
			ParameterEnumBase* enum_parameter = rtti_cast<ParameterEnumBase>(&parameter);

			const rtti::TypeInfo& enum_type = enum_parameter->getEnumType();
			assert(enum_type.is_enumeration());

			rttr::enumeration enum_instance = enum_type.get_enumeration();
			std::vector<rttr::string_view> items(enum_instance.get_names().begin(), enum_instance.get_names().end());

			int value = enum_parameter->getValue();
			if (ImGui::Combo(parameter.mID.c_str(), &value, [](void* data, int index, const char** out_text)
			{
				std::vector<rttr::string_view>* items = (std::vector<rttr::string_view>*)data;
				*out_text = (*items)[index].data();
				return true;
			}, &items, items.size()))
			{
				enum_parameter->setValue(value);
			}
		});
	}

	void ParameterGUI::handleLoadPopup()
	{
		if (ImGui::BeginPopupModal("Load", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Combo("Presets", &mSelectedPresetIndex, [](void* data, int index, const char** out_text)
			{
				ParameterService::PresetFileList* preset_files = (ParameterService::PresetFileList*)data;
				*out_text = (*preset_files)[index].data();
				return true;
			}, &mPresets, mPresets.size());

			if (ImGui::Button("OK"))
			{
				utility::ErrorState errorState;
				if (mParameterService.loadPreset(mPresets[mSelectedPresetIndex], errorState))
					ImGui::CloseCurrentPopup();
				else
					ImGui::OpenPopup("Failed to load preset");

				if (ImGui::BeginPopupModal("Failed to load preset"))
				{
					ImGui::Text(errorState.toString().c_str());
					if (ImGui::Button("OK"))
					{
						ImGui::CloseCurrentPopup();
					}

					ImGui::EndPopup();
				}
			}

			ImGui::SameLine();

			if (ImGui::Button("Cancel"))
			{
				restorePresets();
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}

	bool ParameterGUI::handleNewPopup(std::string& outNewFilename)
	{
		bool result = false;

		if (ImGui::BeginPopupModal("New", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			static char name[256] = { 0 };
			ImGui::InputText("Name", name, 256);

			if (ImGui::Button("OK") && strlen(name) != 0)
			{
				outNewFilename = std::string(name, strlen(name));
				outNewFilename += ".json";
				ImGui::CloseCurrentPopup();
				result = true;
			}

			ImGui::SameLine();

			if (ImGui::Button("Cancel"))
				ImGui::CloseCurrentPopup();

			ImGui::EndPopup();
		}

		return result;
	}

	void ParameterGUI::handleSaveAsPopup()
	{
		if (ImGui::BeginPopupModal("Save As", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			if (ImGui::Combo("Presets", &mSelectedPresetIndex, [](void* data, int index, const char** out_text)
			{
				ParameterService::PresetFileList* preset_files = (ParameterService::PresetFileList*)data;
				*out_text = (*preset_files)[index].data();
				return true;
			}, &mPresets, mPresets.size()))
			{
				if (mSelectedPresetIndex == mPresets.size() - 1)
				{
					ImGui::OpenPopup("New");
				}
			}


			std::string newFilename;
			if (handleNewPopup(newFilename))
			{
				// Insert before the '<new...>' item
				mPresets.insert(mPresets.end() - 1, newFilename);
			}

			ImGui::SameLine();

			if (ImGui::Button("OK"))
			{
				utility::ErrorState errorState;
				if (mParameterService.savePreset(mPresets[mSelectedPresetIndex], errorState))
				{
					ImGui::CloseCurrentPopup();
					std::string previous_selection = mPresets[mSelectedPresetIndex];

					mPresets = mParameterService.getPresets();

					// After we have retrieved the filenames from the service, the list may be in a different order,
					// so we search for the item in the list to find the selected index.
					for (int index = 0; index < mPresets.size(); ++index)
					{
						if (mPresets[index] == previous_selection)
						{
							mSelectedPresetIndex = index;
							break;
						}
					}

				}
				else
				{
					ImGui::OpenPopup("Failed to save preset");
				}

				if (ImGui::BeginPopupModal("Failed to save preset"))
				{
					ImGui::Text(errorState.toString().c_str());
					if (ImGui::Button("OK"))
					{
						ImGui::CloseCurrentPopup();
					}

					ImGui::EndPopup();
				}
			}

			ImGui::SameLine();

			if (ImGui::Button("Cancel"))
			{
				restorePresets();
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}

	void ParameterGUI::savePresets()
	{
		mPrevSelectedPresetIndex = mSelectedPresetIndex;
		mPrevPresets = mPresets;
	}

	void ParameterGUI::restorePresets()
	{
		mSelectedPresetIndex = mPrevSelectedPresetIndex;
		mPresets = mPrevPresets;
	}

	void ParameterGUI::showPresets()
	{
		ImGui::Text("Current preset: ");
		ImGui::SameLine();

		bool hasPreset = mSelectedPresetIndex >= 0 && mSelectedPresetIndex < mPresets.size();

		if (hasPreset)
			ImGui::Text(mPresets[mSelectedPresetIndex].data());
		else
			ImGui::Text("<No preset>");

		if (ImGui::Button("Save"))
		{
			if (hasPreset)
			{
				utility::ErrorState errorState;
				if (!mParameterService.savePreset(mPresets[mSelectedPresetIndex], errorState))
					ImGui::OpenPopup("Failed to save preset");

				if (ImGui::BeginPopupModal("Failed to save preset"))
				{
					ImGui::Text(errorState.toString().c_str());
					if (ImGui::Button("OK"))
					{
						ImGui::CloseCurrentPopup();
					}

					ImGui::EndPopup();
				}

			}
			else
			{
				ImGui::OpenPopup("Save As");
				savePresets();
				mPresets.push_back("<New...>");
			}
		}

		ImGui::SameLine();

		if (ImGui::Button("Save As"))
		{
			ImGui::OpenPopup("Save As");
			savePresets();
			mPresets.push_back("<New...>");
		}

		ImGui::SameLine();

		if (ImGui::Button("Load"))
		{
			ImGui::OpenPopup("Load");
			savePresets();
		}

		handleLoadPopup();
		handleSaveAsPopup();
	}

	void ParameterGUI::showParameters(ParameterContainer& parameterContainer, bool isRoot)
	{
		if (isRoot || ImGui::TreeNode(parameterContainer.mID.c_str()))
		{
			for (auto& parameter : parameterContainer.mParameters)
			{
				const rtti::TypeInfo& type = parameter->get_type();

				ParameterEditorMap::iterator pos = mParameterEditors.find(type);
				assert(pos != mParameterEditors.end());

				pos->second(*parameter);
			}
			
			if (!isRoot)
				ImGui::TreePop();

			for (auto& child : parameterContainer.mChildren)
				showParameters(*child, false);
		}
	}


	void ParameterGUI::show()
	{
		ImGui::Begin("Parameters");

		showPresets();

		if (mParameterService.hasParameters())
		{
			ImGui::Separator();
			showParameters(mParameterService.getParameters(), true);
		}

		ImGui::End();
	}

	void ParameterGUI::registerParameterEditor(const rtti::TypeInfo& type, const CreateParameterEditor& createParameterEditorFunc)
	{
		std::vector<rtti::TypeInfo> types;
		types.push_back(type);
		rtti::getDerivedTypesRecursive(type, types);
		
		for (const rtti::TypeInfo& type : types)
			mParameterEditors[type] = createParameterEditorFunc;
	}
}