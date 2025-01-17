/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

// External Inlcludes
#include <appeventhandler.h>
#include <sdleventconverter.h>

namespace nap
{
	// Forward Declares
	class IMGuiService;

	/**
	 * Application event handler that is designed to work with applications that host a graphical user interface.
	 * This class checks if the user is interacting with a GUI element, if so, no input events are forwarded to the application.
	 */
	class NAPAPI GUIAppEventHandler : public AppEventHandler
	{
		RTTI_ENABLE(AppEventHandler)
	public:
		GUIAppEventHandler(App& app);

		/**
		 * This call creates the SDL Input Converter.
		 */
		virtual void start() override;

		/**
		 * This call polls the various SDL messages and filters them based on GUI activity. 
		 * If a GUI element is actively used the events are not forwarded to the running app.
		 */
		virtual void process() override;

		/**
		* This call deletes the input converter
		*/
		virtual void shutdown() override;

	private:
		std::unique_ptr<SDLEventConverter> mEventConverter = nullptr;
		IMGuiService* mGuiService = nullptr;
	};
}