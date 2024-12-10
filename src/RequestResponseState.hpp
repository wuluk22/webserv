#ifndef REQUESTRESPONSESTATE_HPP
# define REQUESTRESPONSESTATE_HPP

#include <cstddef>
# include <iostream>
# include "HttpRequestHandler.hpp"
# include "HttpResponseHandler.hpp"
# include "ServerHandler.hpp"

class	HttpRequestHandler;
class	HttpResponseHandler;
class	ServerHandler;

class	RRState
{
	private:
		HttpRequestHandler				_request;
		HttpResponseHandler				_response;
		std::vector<LocationBlock *>	_locations;
		// ServerHandler					_server;
	
	public:
	//METHODS
	RRState();
	~RRState();

	//GETTER
	HttpRequestHandler&				getRequest();
	HttpResponseHandler&			getResponse();
	std::vector<LocationBlock *>&	getLocations();

	//SETTER
	void				setRequest(HttpRequestHandler& req);
	void				setResponse(HttpResponseHandler& res);
	void				setLocations(std::vector<LocationBlock *>& locations);
	HttpRequestHandler	initRequest(const HttpRequestHandler& request);
};

#endif
