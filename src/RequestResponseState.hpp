#ifndef REQUESTRESPONSESTATE_HPP
# define REQUESTRESPONSESTATE_HPP

# include <cstddef>
# include <iostream>
# include "HttpRequestHandler.hpp"
# include "HttpResponseHandler.hpp"

class	HttpRequestHandler;
class	HttpResponseHandler;

class	RRState
{
	private:
		HttpRequestHandler	_request;
		HttpResponseHandler	_response;
	
	public:
	//METHODS
	RRState();
	~RRState();

	//GETTER
	HttpRequestHandler&		getRequest();
	HttpResponseHandler&	getResponse();
;	
	//SETTER
	void				setRequest(HttpRequestHandler& req);
	void				setResponse(HttpResponseHandler& res);
};

#endif
