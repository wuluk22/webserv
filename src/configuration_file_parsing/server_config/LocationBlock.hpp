#ifndef LOCATION_BLOCK_HPP
# define LOCATION_BLOCK_HPP

#include <vector>
#include <iostream>
#include "ADirective.hpp"

// ERROR STATUS CODE DEFAULT VALUE
#define NO_RETURN 999

struct s_return {
	std::size_t	_status_code;
	std::string _redirection_url;
};

struct s_loc_params {
	s_return		_return_args;
	std::string		_cgi_path;
	std::string		_uri;
	std::string		_content_path;
	std::string		_uri_dependance;
	unsigned char	_allowed_methods;
	bool			_modified_client_max_body_size;
	bool			_modified_auto_index;
};

enum e_allowed_methods {
	GET = 1 << 0,
	POST = 1 << 1,
	DELETE = 1 << 2
};

class LocationBlock : public ADirective {
	private:
		s_loc_params _location_params;
		LocationBlock* _upper_location;
	public:
		LocationBlock(void);
		~LocationBlock();
		s_loc_params	getLocationParams(void) const;

		// Data reachability

		e_data_reach 							isContentPathReachable(void);
		e_data_reach 							isCgiPathReachable(void);
		std::pair<std::string, e_data_reach>	checkAvailableIndex(void);

		// Setter
		void setUriDependance(std::string uri);
		void clientMaxBodySizeModified(void);
		void autoIndexModified(void);
		bool setCgiPath(std::string path_args);
		bool setUri(std::string uri_args, std::string root);
		bool setAlias(std::string alias_path);
		bool setAllowedMethods(unsigned char allowed_method);
		bool setContentPath(std::string content_path);
		void setReturnArgs(std::size_t status_code, std::string redirection_url);
		
        // Getter
		std::string					getCgiPath(void) const;
		// std::string				getAlias(void) const;
		std::string					getUri(void) const;
		std::string					getContentPath(void) const;
		s_return					getReturnArgs(void) const;
		std::vector<std::string>	accessibleIndex(void);
		bool						isCgiAllowed(void) const;
		// bool						isDirectiveCgi(void) const;
		bool						isGetAllowed(void) const;
		bool						isPostAllowed(void) const;
		bool						isDeleteAllowed(void) const;
		bool						hasClientMaxBodySizeModified(void) const;
		bool						hasAutoIndexModified(void) const;
		std::string					getUriDependance(void) const;
};

std::ostream& operator<<(std::ostream& os, const LocationBlock *location_params);

#endif