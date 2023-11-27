#pragma once

#include "Webserv.hpp"

bool ft_isAllSpace(std::string &s);

struct parsingStruct
{
	std::string name;
	bool open_bracket;
	bool close_bracket;
};

struct serverConfigData
{
	int listen;				  // +
	int root;				  // +
	int client_body_max_size; // +
	int redirect;			  // ~
	int index;				  // ~
	int autoindex;			  // ~
	int upload_dir;			  // ~
};

struct locationConfigData
{
	int listen;				  // --
	int root;				  // +
	int client_body_max_size; // +
	int mathods;			  // +
	int redirect;			  // ~
	int index;				  // ~
	int autoindex;			  // ~
	int upload_dir;			  // ~
};

class Parsing
{
	public:
		std::vector< std::vector<std::string > > servers;
		std::map< std::string, std::vector<std::string > > responseTypes;
		std::vector< std::string > httpStatusCodes;
		std::ifstream configFile;
		std::string fileName;

		Parsing();
		Parsing(const Parsing &copy);
		Parsing &operator=(const Parsing &copy);
		std::vector<std::string> getHttpStatusCodes();
		bool parsing(int ac, char **av);
		bool fillServerList();
		void fillResponseTypes();
		bool finalConfigFileParsing();

		std::string getMimeTypes(const std::string &);
		size_t getServersNumber();
		size_t getLocationsNumber(size_t serverIndex);
		int checkForLocation(size_t serverIndex, const std::string &);

		bool checkRepetedServers();
		std::vector < std::string > getAllCGI();

		// get all of this ==>> listen,  server_name,  autoindex,  upload_dir,  root,  client_body_max_size
		std::vector < std::pair < std::string, std::vector < std::string > > > serverData(size_t index);

		// get one of this ==>> listen,  server_name,  autoindex,  upload_dir,  root,  client_body_max_size
		std::string getServerDataSingle(size_t serverIndex, std::string data);

		// get server error pages
		std::vector < std::pair < std::string, std::string > > getServerErrorPages(size_t index);

		// get all location data
		std::vector < std::pair < std::string, std::vector < std::string > > > getLocationData(size_t serverIndex, size_t locationIndex);

		// get one of location data ==> root, index, upload_dir
		std::vector< std::string > getLocationSingle(size_t serverIndex, size_t locationIndex, std::string data);

		// get one of location data ==> methods, cgi, redirect
		std::vector< std::pair < std::string, std::string > > getLocationMultiple(size_t serverIndex, size_t locationIndex, std::string data);
};



bool checkOther(const std::string &val);
bool isValidPort(const std::string &port);
bool isValidIPAddress(const std::string &ipAddress);
bool isDirectory(const std::string &path);
bool isFile(const std::string &path, bool permission);
bool isValidAutoIndex(const std::string &autoindex);
bool isValidUploadDir(const std::string &uploadDir);
bool isValidRoot(const std::string &root);
bool isValidClientBodyMaxSize(const std::string &clientBodyMaxSize);
bool isValidErrorPage(const std::string &err);
bool isValidLocationPath(const std::string &uri);

bool isValidLocationMethods(std::vector<std::string> &data);
bool isValidLocationCGI(const std::string &cgi, const std::string &cgiFile);
bool checkServerData(std::vector<std::string> &data);

