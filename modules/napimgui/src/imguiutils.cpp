/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

// Local Includes
#include "imguiutils.h"
#include "imguiservice.h"

// External Includes
#include <renderservice.h>
#include <nap/core.h>

namespace ImGui
{
	void Image(nap::Texture2D& texture, const ImVec2& size, const ImVec2& uv0 /*= ImVec2(0, 0)*/, const ImVec2& uv1 /*= ImVec2(1, 1)*/, const ImVec4& tint_col /*= ImVec4(1, 1, 1, 1)*/, const ImVec4& border_col /*= ImVec4(0, 0, 0, 0)*/)
	{
		nap::Core& core = texture.getRenderService().getCore();
		nap::IMGuiService* gui_service = core.getService<nap::IMGuiService>();
		ImGui::Image(gui_service->getTextureHandle(texture), size, uv0, uv1, tint_col, border_col);
	}


	ImTextureID IMGUI_API GetTextureHandle(nap::Texture2D& texture)
	{
		nap::Core& core = texture.getRenderService().getCore();
		nap::IMGuiService* gui_service = core.getService<nap::IMGuiService>();
		return gui_service->getTextureHandle(texture);
	}
}