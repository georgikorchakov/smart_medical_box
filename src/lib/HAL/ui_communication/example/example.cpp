// g++ main.cpp ../ui_communication.cpp -lzmq -o main

#include "../ui_communication.h"

int main()
{
	UICommunication ui;

	while (1)
	{
		if (ui.is_req_waiting())
		{
			std::cout << "REQ: " << ui.get_command() << "\n";
			ui.send_command("WORLD");
		}

		std::cout << "Do some work\n";
	}

	return 0;
}
