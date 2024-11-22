#ifndef REQUESTRESPONSESTATE_HPP
# define REQUESTRESPONSESTATE_HPP

# include <iostream>
# include "HttpRequestHandler.hpp"
# include "HttpResponseHandler.hpp"

class	HttpRequestHandler;
class	HttpResponseHandler;

class	RRState
{
	private:
		std::string requestBuffer;
		bool requestComplete;
		std::string responseBuffer;
		size_t bytesSent;
		int _resultRecv;
		HttpRequestHandler	_request;
		HttpResponseHandler	_response;
	
	public:
	//METHODS
	RRState();
	~RRState();

	//GETTER
	// std::string&			getRequestBuffer();
	HttpRequestHandler&		getRequest();
	HttpResponseHandler&	getResponse();
	int&					getResultRecv();
;	
	//SETTER
	void				setRequest(HttpRequestHandler& req);
	void				setResponse(HttpResponseHandler& res);
	void				setResultRecv(int& resultRecv);
};

#endif
