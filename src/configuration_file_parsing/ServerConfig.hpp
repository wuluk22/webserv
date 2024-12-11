#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include <vector>
#include <iostream>
#include <set>
#include <map>

#include "ConfigException.hpp"
#include "PathValidator.hpp"

struct s_common_params {
	std::string				_root;
	std::set <std::string>	_index;
	bool					_auto_index;
	unsigned int			_client_max_body_size;
};

struct s_server_params {
	std::set<std::string>					_server_name;
	std::set<unsigned int>					_listen;
	std::map<unsigned int, std::string>		_error_pages_record;
	std::string								_access_log_file_path;
	std::string								_error_log_file_path;
	std::vector <std::string>				_declared_path;
};

struct s_return {
	std::size_t	_status_code;
	std::string _redirection_url;
};

struct s_loc_params {
	s_return		_return_args;
	std::string		_cgi_path;
	std::string		_uri;
	std::string		_content_path;
	unsigned char	_allowed_methods;
	bool			_modified_client_max_body_size;
	bool			_modified_auto_index;
};

enum e_allowed_methods {
	GET = 1 << 0,
	POST = 1 << 1,
	DELETE = 1 << 2
};

#ifndef IS_LINUX
	#define IS_LINUX 0
#endif

#if defined(__linux__)
    #undef IS_LINUX
    #define IS_LINUX 1
#endif

#define NO_RETURN 999

class LocationBlock;
class ServerBlock;

class ServerConfig {
	private:
		std::vector<LocationBlock *> directives;
		ServerBlock *server_header;
	public:
		ServerConfig(void);
		~ServerConfig();
		std::vector<LocationBlock *>	getDirectives(void) const;
		void							setDirective(LocationBlock *new_directive);
		ServerBlock*					getServerHeader(void) const;
		void							setServerHeader(ServerBlock *server_header);
};

class ADirective {
	protected:
		PathValidator _validator;
		s_common_params _common_params;
	public:
		ADirective(void);
		virtual			~ADirective();
		bool			isValidFileName(std::string filename);
		s_common_params getCommonParams(void) const;

		// Individual setters
		bool setRoot(std::string root_args);
		void setIndex(std::set <std::string> index_args);
		void setAutoIndex(bool value);
		void setClientMaxBodySize(unsigned int body_size_value);

		// Individual getters
		std::string				getRoot(void) const;
		std::set <std::string>	getIndex(void) const;
		bool					getAutoIndex(void) const;
		unsigned int			getClientMaxBodySize(void) const;
};

std::ostream& operator<<(std::ostream& os, const s_common_params *params);

class ServerBlock : public ADirective {
	private:
		s_server_params	_server_params;
		bool 			_listening_ports_set;
		bool			checkDeclaredPathOccurence(std::string path);
	public:
		ServerBlock(void);
		~ServerBlock();
		s_server_params getServerParams(void) const;

		//Individual setters
		void setServerName(std::set<std::string> server_names);
		bool setListeningPort(std::set<unsigned int> listening_ports);
		void setErrorPagesRecord(std::map<unsigned int, std::string> error_pages_record);
		bool setAcessLogPath(std::string path);
		bool setErrorLogPath(std::string path);

		//Individual getter
		std::set<std::string>				getServerName(void) const;
		std::set<unsigned int>				getListeningPort(void) const;
		std::map<unsigned int, std::string>	getErrorPagesRecord(void) const;
		std::string							getAccessLogPath(void) const;
		std::string							getErrorLogPath(void) const;
};

std::ostream& operator<<(std::ostream& os, const ServerBlock *server_params);

class LocationBlock : public ADirective {
	private:
		s_loc_params _location_params;
		LocationBlock* _upper_location;
		bool isValidExtension(std::string extension);
	public:
		LocationBlock(void);
		~LocationBlock();
		s_loc_params	getLocationParams(void) const;
		void			setUpperLocation(LocationBlock* upper_location);
		void			getUpperLocation(void) const;

		// Individual setter
		void clientMaxBodySizeModified(void);
		void autoIndexModified(void);
		bool setCgiPath(std::string path_args);
		bool setUri(std::string uri_args, std::string root);
		bool setAlias(std::string alias_path);
		bool setAllowedMethods(unsigned char allowed_method);
		bool setContentPath(std::string content_path);
		void setReturnArgs(std::size_t status_code, std::string redirection_url);
		// Individual getter
		std::string	getCgiPath(void) const;
		std::string	getAlias(void) const;
		std::string	getUri(void) const;
		std::string	getContentPath(void) const;
		s_return	getReturnArgs(void) const;
		std::string	getFirstAccessibleIndex(void);
		bool		isCgiAllowed(void) const;
		bool		isDirectiveCgi(void) const;
		bool		isGetAllowed(void) const;
		bool		isPostAllowed(void) const;
		bool		isDeleteAllowed(void) const;
		bool		hasClientMaxBodySizeModified(void) const;
		bool		hasAutoIndexModified(void) const;
};

std::ostream& operator<<(std::ostream& os, const LocationBlock *location_params);

#endif