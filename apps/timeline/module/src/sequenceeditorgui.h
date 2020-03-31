#pragma once

// internal includes
#include "sequenceeditor.h"

// external includes
#include <nap/resource.h>
#include <nap/resourceptr.h>
#include <rtti/objectptr.h>
#include <imgui/imgui.h>

namespace nap
{
	//////////////////////////////////////////////////////////////////////////

	// forward declares
	class SequenceEditorGUIView;
	class SequenceEditorView;

	/**
	 */
	class NAPAPI SequenceEditorGUI : public Resource
	{
		RTTI_ENABLE(Resource)
	public:
		virtual bool init(utility::ErrorState& errorState);

		virtual void onDestroy();

		void draw();
	public:
		ResourcePtr<SequenceEditor> mSequenceEditor = nullptr;
	protected:
		std::unique_ptr<SequenceEditorGUIView> mView = nullptr;
	};


	enum SequenceGUIMouseActions
	{
		// ACTIONS
		DRAGGING_SEGMENT,
		INSERTING_SEGMENT,
		OPEN_INSERT_SEGMENT_POPUP,
		DELETING_SEGMENT,
		OPEN_DELETE_SEGMENT_POPUP,
		HOVERING_SEGMENT,
		HOVERING_SEGMENT_VALUE,
		DRAGGING_SEGMENT_VALUE,
		HOVERING_CONTROL_POINT,
		DRAGGING_CONTROL_POINT,
		HOVERING_TAN_POINT,
		DRAGGING_TAN_POINT,
		HOVERING_CURVE,
		NONE
	};

	class SequenceGUIActionData
	{
	public:
		SequenceGUIActionData() {}
		virtual ~SequenceGUIActionData() {}
	};

	class SequenceGUIDragSegmentData : public SequenceGUIActionData
	{
	public:
		SequenceGUIDragSegmentData(
			std::string trackId_, 
			std::string segmentID_, 
			SegmentValueTypes type_)
			: trackID(trackId_),
				segmentID(segmentID_),
				type(type_) {}

		std::string			trackID;
		std::string			segmentID;
		SegmentValueTypes	type;
	};

	class SequenceGUIDragTanPointData : public SequenceGUIActionData
	{
	public:
		SequenceGUIDragTanPointData(
			std::string trackId_, 
			std::string segmentID_,
			int controlPointIndex_,
			TanPointTypes type_)
			:	trackID(trackId_), 
				segmentID(segmentID_), 
				controlPointIndex(controlPointIndex_), 
				type(type_) {}

		std::string		trackID;
		std::string		segmentID;
		int				controlPointIndex;
		TanPointTypes	type;
	};

	class SequenceGUIDragControlPointData : public SequenceGUIActionData
	{
	public:
		SequenceGUIDragControlPointData(std::string trackId_, std::string segmentID_, int controlPointIndex_) 
			: trackID(trackId_), segmentID(segmentID_), controlPointIndex(controlPointIndex_){}

		std::string trackID;
		std::string segmentID;
		int			controlPointIndex;
	};

	class SequenceGUIDeleteSegmentData : public SequenceGUIActionData
	{
	public:
		SequenceGUIDeleteSegmentData(std::string trackId_, std::string segmentID_) : trackID(trackId_), segmentID(segmentID_) {}

		std::string trackID;
		std::string segmentID;
	};

	class SequenceGUIInsertSegmentData : public SequenceGUIActionData
	{
	public:
		SequenceGUIInsertSegmentData(std::string id, double t) : trackID(id), time(t) {}

		double time = 0.0;
		std::string trackID;
	};

	class SequenceGUIState
	{
	public:
		SequenceGUIMouseActions currentAction = SequenceGUIMouseActions::NONE;
		std::string currentObjectID = "";
		std::unique_ptr<SequenceGUIActionData> currentActionData;
	};


	/**
	 */
	class SequenceEditorView
	{
	public:
		// constructor
		SequenceEditorView(const Sequence& sequence, SequenceEditorController& controller);
	protected:
		const Sequence& mSequence;
		SequenceEditorController& mController;
	};


	/**
	 */
	class SequenceEditorGUIView : public SequenceEditorView
	{
	public:
		SequenceEditorGUIView(
			const Sequence& sequence,
			SequenceEditorController& controller,
			std::string id);

		void draw();

	private:
		void drawTracks(
			const Sequence &sequence,
			const float timelineWidth,
			const ImVec2 &mousePos,
			const float stepSize,
			const ImVec2 &mouseDelta);

		void drawSegmentValue(
			const SequenceTrack& track,
			const SequenceTrackSegment& segment,
			const ImVec2 &trackTopLeft,
			const float segmentX,
			const float segmentWidth,
			const float trackHeight,
			const ImVec2 &mouseDelta,
			const float stepSize,
			const SegmentValueTypes segmentType,
			ImDrawList* drawList);

		void drawSegmentHandler(
			const SequenceTrack& track,
			const SequenceTrackSegment& segment,
			const ImVec2 &trackTopLeft,
			const float segmentX,
			const float segmentWidth,
			const float trackHeight,
			const ImVec2 &mouseDelta,
			const float stepSize,
			ImDrawList* drawList
		);

		void drawCurve(
			const SequenceTrack& track,
			const SequenceTrackSegment& segment,
			const ImVec2 &trackTopLeft,
			const float previousSegmentX,
			const float segmentWidth,
			const float trackHeight,
			const float segmentX,
			const float stepSize,
			ImDrawList* drawList);

		void drawControlPoints(
			const SequenceTrack& track,
			const SequenceTrackSegment& segment,
			const ImVec2 &trackTopLeft,
			const float segmentX,
			const float segmentWidth,
			const float trackHeight,
			const ImVec2 mouseDelta,
			const int stepSize,
			ImDrawList* drawList);

		void drawTanHandler(
			const SequenceTrack &track,
			const SequenceTrackSegment &segment,
			std::ostringstream &stringStream,
			const float segmentWidth,
			const math::FCurvePoint<float, float> &curvePoint,
			const float trackHeight,
			const ImVec2 &circlePoint,
			const int controlPointIndex,
			const TanPointTypes type,
			const ImVec2& mouseDelta,
			const int stepSize,
			ImDrawList* drawList);

		std::string mID;
		SequenceGUIState mState;
		ImVec2 mPreviousMousePos = { 0,0 };
	};
}
