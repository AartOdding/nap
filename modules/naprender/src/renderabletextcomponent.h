#pragma once

// Local Includes
#include "rendercomponent.h"
#include "materialinstance.h"
#include "renderableglyph.h"
#include "uniforminstance.h"

// External Includes
#include <font.h>
#include <planemesh.h>
#include <renderablemesh.h>
#include <transformcomponent.h>

namespace nap
{
	class RenderableTextComponentInstance;

	/**
	 * Render-able Text Component Resource.
	 * Creates a RenderableTextComponentInstance that can draw text using a font and material.
	 * Text rendering works best when the blend mode of the Material is set to: AlphaBlend and the Depth mode to NoReadWrite.
	 * This ensures the text is rendered on top of the rest and remains visible.
	 * Use the Renderable2DTextComponent to render text in screen space and the Renderable3DTextComopnent to render text in 3D space.
	 *
	 * The nap::RenderableTextComponentInstance can cache multiple lines at once, where each line can be selected and drawn individually inside a render loop.
	 * This is useful when you want the same component to render multiple lines of text, removing the need to declare a component for each individual line.
	 * You cannot update or add a line of text when rendering a frame: inside the render loop.
	 * Only update or add new lines of text on application update. You can however change the position and line of text to draw inside the render loop.
	 *
	 * For example, on update:
	 * ~~~~~
	 *		// Set text for next draw operation
	 *		text_component.resize(blobs.size());
	 *		for (int i = 0; i < blobs.size(); i++)
	 *		{
	 *			text_component.setText(i, utility::stringFormat("Blob %d", i + 1), error);
	 *		}
	 * ~~~~~
	 *
	 * And on render:
	 * ~~~~~
	 *		// Set text for next draw operation
	 *		for (int i = 0; i < blobs.size(); i++)
	 *		{
	 *			// Get blob location in screen space
	 *			glm::vec3 blob_pos = locs[i];
	 *			glm::vec2 text_pos = persp_camera.worldToScreen(blob_pos, mRenderWindow->getRectPixels());
	 *
	 *			// Set location, select line and draw
	 *			text_comp.setLocation(text_pos);
	 *			text_comp.setLineIndex(i);
	 *			text_comp.draw(*mRenderWindow)
	 *		}
	 * ~~~~~
	 *
	 */
	class NAPAPI RenderableTextComponent : public RenderableComponent
	{
		RTTI_ENABLE(RenderableComponent)
		DECLARE_COMPONENT(RenderableTextComponent, RenderableTextComponentInstance)
	public:
		ResourcePtr<Font> mFont;								///< Property: 'Font' that represents the style of the text
		std::string mText;										///< Property: 'Text' to draw
		MaterialInstanceResource mMaterialInstanceResource;		///< Property: 'MaterialInstance' the material used to shade the text
		std::string mGlyphUniform = "glyph";					///< Property: 'GlyphUniform' name of the 2D texture character binding in the shader, defaults to 'glyph'
	};


	/**
	 * Draws text into the currently active render target using a font and material.
	 * This is the runtime version of the RenderableTextComponent resource.
	 * Text rendering works best when the blend mode of the Material is set to: AlphaBlend and the Depth mode to NoReadWrite. 
	 * This ensures the text is rendered on top of the rest and remains visible.
	 * Use the Renderable2DTextComponent to render text in screen space and the Renderable3DTextComopnent to render text in 3D space.
	 *
	 * It is possible to cache multiple lines at once, where each line can be selected and drawn individually inside a render loop.
	 * This is useful when you want the same component to render multiple lines of text, removing the need to declare a component for each individual line. 
	 * You cannot update or add a line of text when rendering a frame: inside the render loop.
	 * Only update or add new lines of text on application update. You can however change the position and line of text to draw inside the render loop.
	 *
	 * For example, on update:
	 * ~~~~~
	 *		// Set text for next draw operation
	 *		text_component.resize(blobs.size());
	 *		for (int i = 0; i < blobs.size(); i++)
	 *		{
	 *			text_component.setText(i, utility::stringFormat("Blob %d", i + 1), error);
	 *		}
	 * ~~~~~
	 *
	 * And on render:
	 * ~~~~~
	 *		// Set text for next draw operation
	 *		for (int i = 0; i < blobs.size(); i++)
	 *		{
	 *			// Get blob location in screen space
	 *			glm::vec3 blob_pos = locs[i];
	 *			glm::vec2 text_pos = persp_camera.worldToScreen(blob_pos, mRenderWindow->getRectPixels());
	 *
	 *			// Set location, select line and draw
	 *			text_comp.setLocation(text_pos);
	 *			text_comp.setLineIndex(i);
	 *			text_comp.draw(*mRenderWindow)
	 *		}
	 * ~~~~~
	 * 
	 */
	class NAPAPI RenderableTextComponentInstance : public RenderableComponentInstance
	{
		RTTI_ENABLE(RenderableComponentInstance)
	public:

		RenderableTextComponentInstance(EntityInstance& entity, Component& resource);

		/**
		 * Initializes the this component.
		 * @param errorState holds the error message when initialization fails
		 * @return if the component initialized successfully
		 */
		virtual bool init(utility::ErrorState& errorState) override;

		/**
		 * @return the font used to display text.
		 */
		const FontInstance& getFont() const;

		/**
		 * Set the text to draw at the current line index.
		 * Call setLineIndex() before this call to ensure the right index is updated.
		 * Only set or change text on app update, not render.
		 * @param text the new line of text to draw.
		 * @param error list of unsupported characters
		 * @return if all characters in the text are supported and can be drawn.
		 */
		bool setText(const std::string& text, utility::ErrorState& error);

		/**
		 * Set the text to draw at the given line index. 
		 * Only set or change text on app update, not render.
		 * @param text the new line of text to draw.
		 * @param error list of unsupported characters
		 * @return if all characters in the text are supported and can be drawn.
		 */
		bool setText(int lineIndex, const std::string& text, utility::ErrorState& error);

		/**
		 * Adds a new line of text to the end of the cache. Only allowed on app update, not render.
		 * Current selection will point to he newly added line after this call.
		 * @param text the line of text to add
		 * @param error list of unsupported characters
		 * @return if all characters in the text are supported and can be drawn. 
		 */
		bool addLine(const std::string& text, utility::ErrorState& error);

		/**
		 * Sets the current line index.
		 * The index controls what line of the cache is rendered on draw() and updated when calling setText().
		 * A typical use case would be to set / cache a line of text on app update and draw every line at a different location on render.
		 * @param index new line index.
		 */
		void setLineIndex(int index);

		/**
		 * Returns the text associated with the current line index selection.
		 * @return text associated with current line index.
		 */
		const std::string& getText();

		/**
		 * Returns the text associated with the given line index.
		 * @param index the line index.
		 * @return text associated with the given line index.
		 */
		const std::string& getText(int index);

		/**
		 * Resizes the cache to the given number of lines.
		 * Lines can be drawn / updated individually. 
		 * @param lines new number of lines the cache can hold. 
		 */
		void resize(int lines);

		/**
		 * Returns the total number of available lines to update or draw.
		 * @return total number of available lines to update or draw. 
		 */
		int getCount() const;

		/**
		 * Clears the entire line cache.
		 */
		void clear();

		/**
		 * Returns the bounding box of the current line selection in pixels.
		 * @return the bounding box of the current line selection in pixels.
		 */
		const math::Rect& getBoundingBox() const;

		/**
		 * @param index the line index to get the bounding box for.
		 * @return bounding box in pixels, associated with the given line index.
		 */
		const math::Rect& getBoundingBox(int index);

		/**
		 * Needs to be implemented by derived classes. Creates a RenderableGlyph for the given index in the font.
		 * @param index the index to create the render-able glyph for.
		 * @param error contains the error if the glyph representation could not be created.
		 * @return the render-able glyph for the given character index.
		 */
		virtual RenderableGlyph* getRenderableGlyph(uint index, utility::ErrorState& error) const = 0;

	protected:
		/**
		 * Draws the text into to active render target using the provided matrices.
		 * Call this in derived classes based on extracted matrices.
		 * @param viewMatrix the camera world space location
		 * @param projectionMatrix the camera projection matrix
		 * @param modelMatrix the location of the text in the world
		 */
		void draw(IRenderTarget& renderTarget, VkCommandBuffer commandBuffer, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const glm::mat4& modelMatrix);

		/**
		 * @return if this text has a transform component associated with it
		 */
		bool hasTransform() const										{ return mTransform != nullptr; }

		/**
		 * @return the transform component, nullptr if not initialized or not found
		 */
		const nap::TransformComponentInstance* getTransform() const		{ return mTransform; }			

		FontInstance* mFont = nullptr;									///< Pointer to the font, set on initialization
		RenderService* mRenderService = nullptr;						///< Pointer to the Renderer

	private:
		int mIndex = 0;													///< Current line index to update or draw
		MaterialInstance mMaterialInstance;								///< The MaterialInstance as created from the resource. 
		PlaneMesh mPlane;												///< Plane used to draws a single letter
		std::string mGlyphUniformName = "glyph";						///< Name of the 2D texture character binding in the shader
		Sampler2DInstance* mGlyphUniform = nullptr;						///< Found glyph uniform
		UniformMat4Instance* mModelUniform = nullptr;					///< Found model matrix uniform input
		UniformMat4Instance* mViewUniform = nullptr;					///< Found view matrix uniform input
		UniformMat4Instance* mProjectionUniform = nullptr;				///< Found projection uniform input
		TransformComponentInstance* mTransform = nullptr;				///< Transform used to position text
		RenderableMesh mRenderableMesh;									///< Valid Plane / Material combination
		VertexAttribute<glm::vec3>* mPositionAttr = nullptr;			///< Handle to the plane vertices
		std::vector<math::Rect> mTextBounds;							///< Bounds of the text in pixels
		std::vector<std::vector<RenderableGlyph*>> mGlyphCache;			///< All available lines of text to render
		std::vector<std::string> mLinesCache;							///< All current lines to be drawn
	};
}
