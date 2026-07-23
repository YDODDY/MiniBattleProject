#pragma once

#include "Event.h"
#include <functional>
#include <typeindex>
#include <unordered_map>
#include <memory>
#include <vector>

class EventBus
{
public : 

	template<typename EventType>
	using Callback = std::function<void(const EventType&)>;

	template<typename EventType>
	void Subscribe(Callback<EventType> callback)
	{
		GetCallbacks<EventType>().push_back(std::move(callback));
	}

	template<typename EventType>
	void Publish(const EventType& eventData)
	{
		for (const auto& callback : GetCallbacks<EventType>())
		{
			callback(eventData);
		}
	}

private:

	class ICallbackList
	{
	public: 
		virtual ~ICallbackList() = default;
	};

	template<typename EventType>
	class CallbackList : public ICallbackList
	{
	public : 
		std::vector<Callback<EventType>> callbacks;
	};

	std::unordered_map <
		std::type_index,
		std::unique_ptr<ICallbackList>
	>callbackLists;

	template<typename EventType>
	std::vector<Callback<EventType>>& GetCallbacks()
	{
		std::type_index eventTypeKey(typeid(EventType));

		auto it = callbackLists.find(eventTypeKey);
		if (it == callbackLists.end())
		{
			callbackLists[eventTypeKey] =
				std::make_unique<CallbackList<EventType>>();
		}

		using ListType = CallbackList<EventType>;
		ICallbackList* basePointer =
			callbackLists[eventTypeKey].get();
		ListType* realPointer =
			static_cast<ListType*>(basePointer);

		return realPointer->callbacks;
	}

};
