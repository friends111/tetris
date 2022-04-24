#include "MessageSystem.h"

namespace Tetris
{
	void MessageSystem::addMessage(std::string text, int duration)
	{
		Message msg;
		msg.text = text;
		msg.duration = duration;
		msg.time_counter = 0;
		this->messages.push_front(msg);
	}

	void MessageSystem::process()
	{
		this->messages.remove_if(
			[](const Message& msg)
			{
				return msg.time_counter >= msg.duration;
			}
		);

		for (Message& msg : this->messages)
		{
			msg.time_counter++;
		}
	}

	std::list<MessageSystem::Message> MessageSystem::getMessages()
	{
		return this->messages;
	}
}
