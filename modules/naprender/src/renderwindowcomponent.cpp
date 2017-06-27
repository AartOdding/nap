#include "renderwindowcomponent.h"
#include "nap\windowevent.h"

RTTI_BEGIN_CLASS(nap::RenderWindowResource)
	RTTI_PROPERTY("Width",			&nap::RenderWindowResource::mWidth,			nap::rtti::EPropertyMetaData::Default)
	RTTI_PROPERTY("Height",			&nap::RenderWindowResource::mHeight,		nap::rtti::EPropertyMetaData::Default)
	RTTI_PROPERTY("Borderless",		&nap::RenderWindowResource::mBorderless,	nap::rtti::EPropertyMetaData::Default)
	RTTI_PROPERTY("Resizable",		&nap::RenderWindowResource::mResizable,		nap::rtti::EPropertyMetaData::Default)
	RTTI_PROPERTY("Title",			&nap::RenderWindowResource::mTitle,			nap::rtti::EPropertyMetaData::Default)
RTTI_END_CLASS

namespace nap
{
	RenderWindowResource::RenderWindowResource(RenderService& renderService) :
		mRenderService(&renderService)
	{
	}


	RenderWindowResource::~RenderWindowResource()
	{
		if (mWindow != nullptr)
			mRenderService->removeWindow(*this);
	}


	bool RenderWindowResource::init(utility::ErrorState& errorState)
	{
		// Let the renderservice create a window
		mWindow = mRenderService->addWindow(*this, errorState);
		if (!errorState.check(mWindow != nullptr, "Failed to create window"))
			return false;

		// We want to respond to resize events for this window
		onEvent.connect(std::bind(&RenderWindowResource::handleEvent, this, std::placeholders::_1));
		return true;
	}


	void RenderWindowResource::handleEvent(const Event& event)
	{
		// Update window size when resizing
		const WindowResizedEvent* resized_event = rtti_cast<const WindowResizedEvent>(&event);
		if (resized_event != nullptr)
		{
			mWindow->setSize(glm::ivec2(resized_event->mWidth, resized_event->mHeight));
		}
	}

	//////////////////////////////////////////////////////////////////////////

	// Constructor
	RenderWindowComponent::RenderWindowComponent()
	{
		// Initialize delta time
		mDeltaTime = NanoSeconds(0);

		activate.setFlag(nap::ObjectFlag::Editable, false);
	}


	// Makes current window active
	void RenderWindowComponent::makeActive()
	{
		mWindow->makeCurrent();
		activate.trigger();
	}


	// Set settings
	void RenderWindowComponent::setConstructionSettings(const RenderWindowSettings& settings)
	{
		mSettings = settings;
	}


	// Shows the window, constructs one if necessary
	void RenderWindowComponent::onShowWindow(const SignalAttribute& signal)
	{
		mWindow->showWindow();
	}


	// Hides the window
	void RenderWindowComponent::onHideWindow(const SignalAttribute& signal)
	{
		mWindow->hideWindow();
	}


	// Occurs when the window title changes
	void RenderWindowComponent::onTitleChanged(AttributeBase& attr)
	{
        mWindow->setTitle(attr.getValue<std::string>());
	}


	// Set Position
	void RenderWindowComponent::onPositionChanged(AttributeBase& attr)
	{
        mWindow->setPosition(attr.getValue<glm::ivec2>());
	}


	// Set Size
	void RenderWindowComponent::onSizeChanged(AttributeBase& attr)
	{
        glm::ivec2 value = attr.getValue<glm::ivec2>();
		mWindow->setSize(value);
		mWindow->setViewport(value);
	}


	// Turn v-sync on - off
	void RenderWindowComponent::onSyncChanged(AttributeBase& attr)
	{
		mWindow->setSync(attr.getValue<bool>());
	}


	// Make window full screen
	void RenderWindowComponent::onFullscreenChanged(AttributeBase& attr)
	{
		mWindow->setFullScreen(attr.getValue<bool>());
	}


	// Registers attributes and pushes current state to window
	void RenderWindowComponent::registered()
	{
		if (!hasWindow())
		{
			nap::Logger::warn(*this, "unable to connect window parameters, no GL Window");
			return;
		}

		// Install listeners on attribute signals
		position.valueChanged.connect(positionChanged);
		size.valueChanged.connect(sizeChanged);
		title.valueChanged.connect(titleChanged);
		sync.valueChanged.connect(syncChanged);
		fullScreen.valueChanged.connect(fullScreenChanged);

		// When visibility changes, hide / show
		show.signal.connect(showWindow);
		hide.signal.connect(hideWindow);

		// Push possible values
		onPositionChanged(position);
		onSizeChanged(size);
		onTitleChanged(title);
		onSyncChanged(sync);
		onFullscreenChanged(fullScreen);

		// Initialize current frame time stamp
		mFrameTimeStamp = mService->getCore().getStartTime();

		// Set start time for fps counter
		mFpsTime = mService->getCore().getElapsedTime();
	}


	// Updates time related values and triggers draw
	void RenderWindowComponent::doDraw()
	{
		// Increment number of rendered frames
		mFrames++;

		// Store amount of time in nanoseconds it took to compute frame
		TimePoint current_time = getCurrentTime();
		mDeltaTime = current_time - mFrameTimeStamp;

		// Update timestamp to be current time
		mFrameTimeStamp = current_time;

		// Update fps
		updateFpsCounter(getDeltaTime());
	}


	// Triggers window update
	void RenderWindowComponent::doUpdate()
	{
	}


	// Return compute last frame compute time
	double RenderWindowComponent::getDeltaTime() const
	{
		return std::chrono::duration<double>(mDeltaTime).count();
	}


	// Frame compute time in float
	float RenderWindowComponent::getDeltaTimeFloat() const
	{
		return std::chrono::duration<float>(mDeltaTime).count();
	}


	float RenderWindowComponent::getFps() const
	{
		return mFps;
	}


	// Updates internal fps counter
	void RenderWindowComponent::updateFpsCounter(double deltaTime)
	{
		mFpsTime += deltaTime;
		if (mFpsTime < 0.1)
			return;

		mFps = (float)((double)mFrames / (mFpsTime));
		mFpsTime = 0.0;
		mFrames = 0;
	}
}

RTTI_DEFINE(nap::RenderWindowComponent)


