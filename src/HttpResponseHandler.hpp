/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponseHandler.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nechaara <nechaara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/08 13:07:27 by clegros           #+#    #+#             */
/*   Updated: 2025/01/09 18:05:36 by nechaara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSEHANDLER_HPP
# define HTTPRESPONSEHANDLER_HPP
# include <iostream>
# include <sstream>
# include <fstream>
# include <map>
# include <vector>
# include <sys/stat.h>
# include <sys/socket.h>
# include <string>
# include <cstdlib>
# include <cstdio>
# include <cstring>
# include <stdexcept>
# include <sys/stat.h>
# include <sys/wait.h>
# include <unistd.h>

class RRState;

class HttpResponseHandler
{
	public:
		void								setHttpVersion(std::string version);
		void								setStatusCode(int code);
		void								setStatusMsg(std::string message);
		void								setHeader(const std::string &headerName, const std::string &headerValue);
		void								setBody(std::string body);
		void								setResponse(std::string output);
		void    							setQuery(std::string query);
		
		std::string							getHttpVersion() const;
		int									getStatusCode() const;
		std::string							getStatusMsg() const;
		std::string							getHeader(const std::string &headerName) const;
		std::map<std::string, std::string>	getHeaders() const;
		std::string							getBody() const;
		std::string							getAll() const;
		std::string							getQuery() const;
		std::string							getPathOfFile(RRState& rrstate);
		
		HttpResponseHandler					handlePath(RRState& rrstate);
		void								handleResponse(RRState& rrstate);
		std::string							urlDecode(const std::string& url);
		HttpResponseHandler					errorHandler(RRState &rrstate, unsigned int error_code, std::string message);
		HttpResponseHandler					handleGet(RRState& rrstate);
		bool								isCgiRequest(RRState& rrstate, const std::string& path);

		std::string							httpVersion;
		int									code;
		std::string							status;
		std::map<std::string, std::string>	headers;
		std::string							body;
		std::string							_query;
};
void				setErrorResponse(RRState& rrstate, int statusCode, const std::string& statusMsg);
std::ostream		&operator<<(std::ostream &out, const HttpResponseHandler &i);

#endif