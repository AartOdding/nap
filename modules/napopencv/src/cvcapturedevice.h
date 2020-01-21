#pragma once

// Local Includes
#include "cvcaptureapi.h"
#include "cvevent.h"
#include "cvadapter.h"
#include "cvservice.h"

// External Includes
#include <nap/device.h>
#include <nap/numeric.h>
#include <nap/resourceptr.h>
#include <nap/signalslot.h>
#include <rtti/factory.h>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
#include <thread>
#include <future>
#include <atomic>
#include <glm/glm.hpp>

namespace nap
{
	/**
	 * Base class of all OpenCV video capture devices, including: video files, image sequences, cameras and web-streams.
	 * Override the various virtual functions to implement an OpenCV capture device.
	 */
	class NAPAPI CVCaptureDevice : public Device
	{
		RTTI_ENABLE(Device)
	public:

		/**
		 * Constructor
		 * @param service the service that receives all frame capture events.
		 */
		CVCaptureDevice(CVService& service);

		/**
		 * Starts the capture device.
		 * @param errorState contains the error if the device can't be started
		 * @return if the device started
		 */
		virtual bool start(utility::ErrorState& errorState) override final;

		/**
		 * Stops the capture device.
		 */
		virtual void stop() override final;

		/**
		 * Grabs the last captured frame if new. The result is stored in the given target.
		 * Internally the frame is removed from the queue.
		 * If there is no new capture the target is not updated and the function returns false.
		 * This operation hands over ownership of the captured frame.
		 * This call is thread safe and can be called every frame in to check for a new frame.
		 * @param target updated to hold the last captured frame data if a new frame is available.
		 * @return if the target is updated with the contents of a new captured frame.
		 */
		bool grab(CVFrameEvent& target);

		/**
		 * Tells the capture thread to capture the next available frame.
		 * Use grab() to check if the new frame is available. 
		 * Typically you call capture after a successful frame grab operation.
 		 */
		void capture();

		/**
		 * Sets an OpenCV capture property. This call is thread safe.
		 * The actual property is applied when a new frame is captured, not immediately.
		 * @param propID the property to set.
		 * @param value the new property value.
		 */
		void setProperty(CVAdapter& adapter, cv::VideoCaptureProperties propID, double value);

		/**
		 * @return the adapter at the given index as type T.
		 */
		template<typename T>
		const T& getAdapter(int index) const;

		/**
		 * @return the adapter at the given index as type T.
		 */
		template<typename T>
		T& getAdapter(int index);

		/**
		 * Returns the last known capture task compute time in seconds.
		 * This includes the time it takes to complete the entire process cycle for all the adapters.
		 * @return capture task compute time.
		 */
		double getComputeTime() const;

		/**
		 * Occurs when a new frame is captured on the background thread.
		 * Listen to this signal when you want to process frame data on a background thread.
		 * Use the CVFrameEvent::copyTo() method to duplicate the complete frame content safely.
		 */
		nap::Signal<const CVFrameEvent&> mFrameCaptured;

		std::vector<nap::ResourcePtr<CVAdapter>> mAdapters;		///< Property: 'Adapters' all the video capture adapters.								{ }
		bool					mAutoCapture = false;			///< Property: 'AutoCapture' if this device captures new frames automatically

	private:
		CVFrameEvent			mCaptureMat;					///< The GPU / CPU matrix that holds the most recent captured video frame
		std::atomic<bool>		mCaptureFrame	= { true };		///< Proceed to next frame
		std::atomic<bool>		mFrameAvailable = { false };	///< If a new frame is captured
		std::atomic<double>		mComputeTime = {0.0f};			///< Last known capture task compute time
		bool					mStopCapturing = false;			///< Signals the capture thread to stop capturing video

		std::future<void>		mCaptureTask;					///< The thread that monitor the read thread
		std::mutex				mCaptureMutex;					///< The mutex that safe guards the capture thread
		std::condition_variable	mCaptureCondition;				///< Used for telling the polling task to continue

		using PropertyMap = std::unordered_map<int, double>;
		std::unordered_map<CVAdapter*, PropertyMap> mPropertyMap;

		CVService*				mService = nullptr;				///< OpenCV service

		/**
		 * Task that captures new frames
		 */
		void captureTask();
	};

	using CVCaptureDeviceObjectCreator = rtti::ObjectCreator<CVCaptureDevice, CVService>;


	//////////////////////////////////////////////////////////////////////////
	// Template definitions
	//////////////////////////////////////////////////////////////////////////

	template<typename T>
	const T& nap::CVCaptureDevice::getAdapter(int index) const
	{
		assert(index < mAdapters.size());
		T* adapter = rtti_cast<T>(mAdapters[index].get());
		assert(adapter != nullptr);
		return *adapter;
	}


	template<typename T>
	T& nap::CVCaptureDevice::getAdapter(int index)
	{
		assert(index < mAdapters.size());
		T* adapter = rtti_cast<T>(mAdapters[index].get());
		assert(adapter != nullptr);
		return *adapter;
	}
}
