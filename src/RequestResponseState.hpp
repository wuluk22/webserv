#ifndef REQUESTRESPONSESTATE_HPP
#define REQUESTRESPONSESTATE_HPP

#include <iostream>

class	RRState
{
	private:
		std::string requestBuffer;
		bool requestComplete;
		std::string responseBuffer;
		size_t bytesSent;
	
	public:
	//METHODS
	RRState();
	~RRState();

	//GETTER
	std::string getRequestBuffer();
	
	//SETTER
};

#endif