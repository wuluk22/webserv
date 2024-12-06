#ifndef REQUESTRESPONSESTATE_HPP
# define REQUESTRESPONSESTATE_HPP

#include <cstddef>
# include <iostream>
# include "HttpRequestHandler.hpp"
# include "HttpResponseHandler.hpp"

class	HttpRequestHandler;
class	HttpResponseHandler;

class	RRState
{
	private:
		HttpRequestHandler				_request;
		HttpResponseHandler				_response;
		std::vector<LocationBlock *>	_locations;
	
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
