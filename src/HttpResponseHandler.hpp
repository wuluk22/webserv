/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponseHandler.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clegros <clegros@student.s19.be>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/08 13:07:27 by clegros           #+#    #+#             */
/*   Updated: 2024/12/06 11:19:25 by clegros          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSEHANDLER_HPP
# define HTTPRESPONSEHANDLER_HPP
# include <iostream>
# include <sstream>
# include <map>
# include "HttpRequestHandler.hpp"
# include <sys/stat.h> // For stat()
# include <string>
# include <iostream>
# include <string>
# include <cstdlib>

class HttpRequestHandler;

class HttpResponseHandler
{
	public:
		void		setHttpVersion(std::string version);
		void		setStatusCode(int code);
		void		setStatusMsg(std::string message);
		void		setHeader(const std::string &headerName, const std::string &headerValue);
		void		setBody(std::string body);
		
		std::string							getHttpVersion() const;
		int									getStatusCode() const;
		std::string							getStatusMsg() const;
		std::string							getHeader(const std::string &headerName) const;
		std::map<std::string, std::string>	getHeaders() const;
		std::string							getBody() const;
		std::string							getAll() const;
		HttpResponseHandler					handlePath(HttpRequestHandler &request, HttpResponseHandler &response);
		void								handleResponse(HttpRequestHandler& request, int clientSock);
		void								sendError(int clientSock, int statusCode, const std::string& statusMsg, const std::string& body);
	private:
		std::string							httpVersion;
		int									code;
		std::string							status;
		std::map<std::string, std::string>	headers;
		std::string							body;
};
std::ostream		&operator<<(std::ostream &out, const HttpResponseHandler &i);
std::string			urlDecode(const std::string& url);
HttpResponseHandler	handleGet(HttpRequestHandler& request, HttpResponseHandler& response);
void				setErrorResponse(HttpRequestHandler& request, HttpResponseHandler& response, int statusCode, const std::string& statusMsg);
bool				isCgiRequest(const std::string& path);
void				handleCgi(HttpRequestHandler& request, HttpResponseHandler& response);

#endif
