/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "helloworldapp.h"

// External Includes
#include <nap/core.h>
#include <nap/logger.h>
#include <renderablemeshcomponent.h>
#include <renderable2dtextcomponent.h>
#include <orthocameracomponent.h>
#include <mathutils.h>
#include <scene.h>
#include <perspcameracomponent.h>
#include <inputrouter.h>
#include <uniforminstance.h>
#include <imguiutils.h>

// Register this application with RTTI, this is required by the AppRunner to 
// validate that this object is indeed an application
RTTI_BEGIN_CLASS_NO_DEFAULT_CONSTRUCTOR(nap::HelloWorldApp)
	RTTI_CONSTRUCTOR(nap::Core&)
RTTI_END_CLASS

namespace nap 
{
	/**
	 * Initialize all the resources and store the objects we need later on
	 */
	bool HelloWorldApp::init(utility::ErrorState& error)
	{		
		// Retrieve services
		mRenderService	= getCore().getService<nap::RenderService>();
		mSceneService	= getCore().getService<nap::SceneService>();
		mInputService	= getCore().getService<nap::InputService>();
		mGuiService		= getCore().getService<nap::IMGuiService>();

		// Get resource manager and load
		mResourceManager = getCore().getResourceManager();
		if (!mResourceManager->loadFile("helloworld.json", error))
			return false;

		// Extract loaded resources
		mRenderWindow = mResourceManager->findObject<nap::RenderWindow>("Window0");
		mWorldTexture = mResourceManager->findObject<nap::ImageFromFile>("WorldTexture");

		// Get the resource that manages all the entities
		ObjectPtr<Scene> scene = mResourceManager->findObject<Scene>("Scene");

		// Fetch world and text
		mWorldEntity = scene->findEntity("World");
		mTextEntity = scene->findEntity("Text");

		// Fetch the two different cameras
		mPerspectiveCamEntity = scene->findEntity("PerspectiveCamera");
		mOrthographicCamEntity = scene->findEntity("OrthographicCamera");

		return true;
	}
	
	
	/**
	 * Forward all the received input messages to the camera input components.
	 * The camera has two input components: KeyInputComponent and PointerInputComponent.
	 * The key input component receives key events, the pointer input component receives pointer events.
	 */
	void HelloWorldApp::update(double deltaTime)
	{
		// The default input router forwards messages to key and mouse input components
		// attached to a set of entities.
		nap::DefaultInputRouter input_router;
		
		// Forward all input events associated with the first window to the listening components
		std::vector<nap::EntityInstance*> entities = { mPerspectiveCamEntity.get() };
		mInputService->processWindowEvents(*mRenderWindow, input_router, entities);

		// Update the camera location in the world shader for the halo effect
		// To do that we fetch the material associated with the world mesh and query the camera location uniform
		// Once we have the uniform we can set it to the camera world space location
		nap::RenderableMeshComponentInstance& render_mesh = mWorldEntity->getComponent<nap::RenderableMeshComponentInstance>();
		nap::UniformStructInstance* ubo = render_mesh.getMaterialInstance().getOrCreateUniform("UBO");
		nap::UniformVec3Instance* cam_loc_uniform = ubo->getOrCreateUniform<nap::UniformVec3Instance>("inCameraPosition");

		// Get camera world space position and set
		nap::TransformComponentInstance& cam_xform = mPerspectiveCamEntity->getComponent<nap::TransformComponentInstance>();
		glm::vec3 global_pos = math::extractPosition(cam_xform.getGlobalTransform());
		cam_loc_uniform->setValue(global_pos);

		// Add some gui elements
		ImGui::Begin("Controls");
		ImGui::Text(getCurrentDateTime().toString().c_str());
		RGBAColorFloat clr = mTextHighlightColor.convert<RGBAColorFloat>();
		ImGui::TextColored(clr, "left mouse button to rotate, right mouse button to zoom");
		ImGui::Text(utility::stringFormat("Framerate: %.02f", getCore().getFramerate()).c_str());

		// Display world texture in GUI
		if (ImGui::CollapsingHeader("Textures"))
		{
			float col_width = ImGui::GetColumnWidth();
			float ratio = (float)mWorldTexture->getHeight() / (float)mWorldTexture->getWidth();
			ImGui::Image(*mWorldTexture, ImVec2(col_width, col_width * ratio));
			ImGui::Text("Word Texture");
		}
		ImGui::End();
	}

	
	/**
	 * Renders the world and text.
	 */
	void HelloWorldApp::render()
	{
		// Signal the beginning of a new frame, allowing it to be recorded.
		// The system might wait until all commands that were previously associated with the new frame have been processed on the GPU.
		// Multiple frames are in flight at the same time, but if the graphics load is heavy the system might wait here to ensure resources are available.
		mRenderService->beginFrame();

		// Begin recording the render commands for the main render window
		if (mRenderService->beginRecording(*mRenderWindow))
		{
			// Begin the render pass
			mRenderWindow->beginRendering();

			// Find the world and add as an object to render
			std::vector<nap::RenderableComponentInstance*> components_to_render;
			nap::RenderableMeshComponentInstance& renderable_world = mWorldEntity->getComponent<nap::RenderableMeshComponentInstance>();
			components_to_render.emplace_back(&renderable_world);

			// Find the perspective camera
			nap::PerspCameraComponentInstance& persp_camera = mPerspectiveCamEntity->getComponent<nap::PerspCameraComponentInstance>();

			// Render the world with the right camera directly to screen
			mRenderService->renderObjects(*mRenderWindow, persp_camera, components_to_render);

			// Locate component that can render text to screen
			Renderable2DTextComponentInstance& render_text = mTextEntity->getComponent<nap::Renderable2DTextComponentInstance>();

			// Center text and render it using the given draw call, 
			// alternatively you can use an orthographic camera to render the text, similar to how the 3D mesh is rendered:  
			// mRenderService::renderObjects(*mRenderWindow, ortho_camera, components_to_render);
			render_text.setLocation({ mRenderWindow->getWidthPixels() / 2, mRenderWindow->getHeightPixels() / 2 });
			render_text.draw(*mRenderWindow);

			// Draw our GUI
			mGuiService->draw();
			
			// End the render pass
			mRenderWindow->endRendering();

			// End recording
			mRenderService->endRecording();
		}

		// Signal the ending of the frame
		mRenderService->endFrame();
	}
	
	
	/**
	 * Occurs when the event handler receives a window message.
	 * You generally give it to the render service which in turn forwards it to the right internal window. 
	 * On the next update the render service automatically processes all window events. 
	 * If you want to listen to specific events associated with a window it's best to listen to a window's mWindowEvent signal
	 */
	void HelloWorldApp::windowMessageReceived(WindowEventPtr windowEvent)
	{
		mRenderService->addEvent(std::move(windowEvent));
	}


	/**
	 * Called by the app loop. It's best to forward messages to the input service for further processing later on
	 * In this case we also check if we need to toggle full-screen or exit the running app
	 */
	void HelloWorldApp::inputMessageReceived(InputEventPtr inputEvent)
	{
		// Escape the loop when esc is pressed
		if (inputEvent->get_type().is_derived_from(RTTI_OF(nap::KeyPressEvent)))
		{
			nap::KeyPressEvent* press_event = static_cast<nap::KeyPressEvent*>(inputEvent.get());
			if (press_event->mKey == nap::EKeyCode::KEY_ESCAPE)
				quit();

			// If 'f' is pressed toggle fullscreen
			if (press_event->mKey == nap::EKeyCode::KEY_f)
			{
				mRenderWindow->toggleFullscreen();
			}
		}

		mInputService->addEvent(std::move(inputEvent));
	}


	int HelloWorldApp::shutdown()
	{
		return 0;
	}
}
