#pragma once
#include <list>
#include <string>

namespace Tetris
{
	class MessageSystem
	{
	public:
		struct Message
		{
			std::string text = "";
			int duration = 60;
			int time_counter = 0;
		};

	private:
		std::list<Message> messages;

	public:
		void addMessage(std::string text, int duration = 60);
		void process();
		std::list<Message> getMessages();
	};
}
