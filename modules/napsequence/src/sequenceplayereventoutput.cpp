/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "sequenceplayereventoutput.h"
#include "sequenceservice.h"

RTTI_BEGIN_CLASS_NO_DEFAULT_CONSTRUCTOR(nap::SequencePlayerEventOutput)
RTTI_END_CLASS

namespace nap
{
	static bool registerObjectCreator = SequenceService::registerObjectCreator([](SequenceService* service)->std::unique_ptr<rtti::IObjectCreator>
	{
		return std::make_unique<SequencePlayerEventInputObjectCreator>(*service);
	});


	SequencePlayerEventOutput::SequencePlayerEventOutput(SequenceService& service)
		: SequencePlayerOutput(service){}


	void SequencePlayerEventOutput::update(double deltaTime)
	{
		std::queue<SequenceEventPtr> events;
		{
			std::lock_guard<std::mutex> lock(mEventMutex);

			// Swap events
			events.swap(mEvents);

			// Clear current queue
			std::queue<SequenceEventPtr> empty_queue;
			mEvents.swap(empty_queue);
		}

		// Keep forwarding events until the queue runs out
		while (!(events.empty()))
		{
			SequenceEventBase& sequence_event = *(events.front());
			mSignal.trigger(sequence_event);

			events.pop();
		}
	}


	void SequencePlayerEventOutput::addEvent(SequenceEventPtr event)
	{
		std::lock_guard<std::mutex> lock(mEventMutex);
		mEvents.emplace(std::move(event));
	}
}