#include "../../includes/Webserv.hpp"

std::string	toString(size_t i)
{
	std::stringstream ss;
    ss << i;
	return ss.str();
}

bool	ft_isAllSpace(std::string& s)
{
	for (size_t i = 0; i < s.size(); i++)
	{
		if (!std::isspace(s[i]))
			return false;
	}
	return true;
}

bool checkForRepetitve(std::string& line)
{
	std::string::iterator it = line.begin();
	std::string::iterator itEnd;

	int cp = 0;
	for (; it != line.end(); it++)
	{
		if (*it == ';')
		{
			cp++;
			if (cp == 1)
				itEnd = it;
		}
	}
	if (cp != 1)
		return false;
	if (itEnd + 1 != line.end())
		return false;
	return true;
}

bool	Parsing::parsing(int ac, char **av)
{
	if (ac > 2)
	{
		std::cerr << "Arrgs Error" << std::endl;
		return false;
	}
	this->fileName = "default.conf";
	if (av[1])
		fileName = av[1];
	this->configFile.open(fileName);
	if (!this->configFile)
	{
		std::cerr << "Unable to open the file " << std::endl;
		return false;
	}

	if (configFile.peek() == std::ifstream::traits_type::eof()) 
	{
		std::cout << "File is empty" << std::endl;
		configFile.close();
		return false;
	}
	std::string					line;
	std::vector <std::string>	pars;
	std::vector <parsingStruct>	parsLast;
	size_t	j = 0;

	bool serverScope = false;
	bool locationScope = false;
	while (std::getline(configFile, line))
	{
		if (line.empty() || ft_isAllSpace(line) || (!line.empty() && ft_trim(line, ' ')[0] == '#'))
			continue;
		pars = ft_split(line);

		if (!pars.size() && !pars[0].empty() && pars[0] != "server" && pars[0] != "location" && pars[0] != "}" && !checkForRepetitve(line))
			return false;
		if (!pars[0].empty() && pars[0] == "server" && pars.size() == 2)
		{
			if (serverScope)
			{
				std::cerr << "Error in the server config " << std::endl;
				return false;
			}
			if (pars.size() != 2 || pars[0] != "server" || pars[1] != "{")
			{
				std::cerr << "Error in the server config " << std::endl;
				return false;
			}
			parsingStruct parsingData = {"server", true, false};
			parsLast.push_back(parsingData);
			serverScope = true;
		}
		else if (!pars[0].empty() && pars[0] == "location" && pars.size() == 3)
		{
			if (serverScope && locationScope)
			{
				std::cerr << "Error in the server location config " << std::endl;
				return false;
			}
			if (pars.back() != "{")
			{
				std::cerr << "Error in the server location config " << std::endl;
				return false;
			}
			parsingStruct parsingData = {"location", true, false};
			parsLast.push_back(parsingData);
			if (serverScope)
				locationScope = true;
		}
		else if (pars.size() == 1 && !pars.front().empty() && pars.front() == "}" && j < parsLast.size())
		{
			parsLast[j++].close_bracket = true;
			if (serverScope && locationScope)
				locationScope = false;
			else if (!locationScope && serverScope)
				serverScope = false;
		}
		else if (!pars.back().empty() && !pars[0].empty() 
			&& pars[0] != "server" && pars[0] != "location" &&  pars.back().back() != ';')
		{
			std::cerr << "Error ; missing" << std::endl;
			return false;
		}
		
		if (!locationScope && !serverScope && pars.front() != "}" && !line.empty())
		{
			std::cerr << "Error somthing out of scope on " << "\033[0;31m"  << line << "\033[0m" << std::endl;
			return false;
		}
	}
	
	for (size_t i = 0; i < parsLast.size(); i++)
	{
		if (parsLast[i].open_bracket == false || parsLast[i].close_bracket == false)
		{
			std::cerr << "Error { or } missing" << std::endl;
			return false;
		}
	}
	configFile.close();
	return true;
}

bool	checkOther(const std::string& val)
{

	std::vector <std::string> data;

	data.push_back("server");
	data.push_back("server_name");
	data.push_back("methods");
	data.push_back("index");
	data.push_back("listen");
	data.push_back("autoindex");
	data.push_back("upload_dir");
	data.push_back("root");
	data.push_back("client_body_max_size");
	data.push_back("error_page");
	data.push_back("location");
	data.push_back("cgi");
	data.push_back("redirect");
	data.push_back("}");
	std::vector <std::string>::iterator it = std::find(data.begin(), data.end(), val);

	if (it == data.end())
		return false;
	
	return true;
}

bool	Parsing::fillServerList()
{
	std::string					line;
	std::string					finalLine;

	this->configFile.open(fileName);
	if (!this->configFile)
	{
		std::cerr << "Unable to open the file " << std::endl;
		return false;
	}

	while (std::getline(configFile, line))
	{
		std::string	trimedLine = ft_trim(line, " \t");
		if ((trimedLine[0] != '#' && trimedLine.find('#') != std::string::npos))
			return false; 
		if (line.empty() || ft_isAllSpace(line) || (!line.empty() && trimedLine[0] == '#'))
			continue;
		if (!checkOther(ft_split(line, " \t")[0]))
			return false; 
		size_t i = trimedLine.find('}');

		std::string eos = "_\n";
		if (i != std::string::npos)
			trimedLine.insert(i + 1, eos);
		finalLine += trimedLine;
	}

	std::vector <std::string>	pars(ft_split(finalLine, "\n{};"));
	std::vector <std::string>	finalParams;
	std::vector <std::string>	params;

	for (size_t i = 0;i < pars.size(); i++)
		finalParams.push_back(ft_trim(pars[i], " \t"));

	for (size_t i = 0; i < finalParams.size(); ++i) 
	{
		if (finalParams[i] == "server") 
		{
			std::vector<std::string> data;

			for (++i; i < finalParams.size(); ++i) 
			{
				if (finalParams[i] == "server") 
				{
					--i; 
					break;
				}
				data.push_back(finalParams[i]);
			}
			this->servers.push_back(data);
		}
	}

	configFile.close();
	pars.clear();
	finalParams.clear();
	params.clear();
	return true;
}

size_t	Parsing::getServersNumber()
{
	return (int)this->servers.size();
}


size_t		Parsing::getLocationsNumber(size_t index)
{
	int	cp = 0;

	if (index < 1 || index > servers.size())
		return -1;
	for (size_t i = 0; i < this->servers.size(); i++)
	{
		if (i == index - 1)
		{
			for (size_t j = 0; j < servers[i].size(); j++)
			{
				if (ft_split(servers[i][j])[0] == "location")
				{
					cp++;
				}
			}
		}
	}
	return cp;
}

std::vector < std::pair <std::string, std::vector < std::string > > > Parsing::getLocationData(size_t serverIndex, size_t locationIndex)
{
	std::vector < std::pair <std::string, std::vector < std::string > > > locationData;
	
	if (getLocationsNumber(serverIndex) < 1)
		return locationData;
	size_t	cp = 0;
	for (size_t i = 0; i < this->servers.size(); i++)
	{
		if (i == serverIndex - 1)
		{
			for (size_t j = 0; j < servers[i].size(); j++)
			{
				std::vector < std::string > line = ft_split(servers[i][j]);
				if (line[0] == "location")
				{
					if (cp == locationIndex - 1)
					{
						while (j < servers[i].size())
						{
							line.clear();
							line = ft_split(servers[i][j]);
							std::vector < std::string > values (line.begin() + 1, line.end());
							locationData.push_back(std::make_pair(line[0], values));
							j++;
							line.clear();
							line = ft_split(servers[i][j]);
							if (j < servers[i].size() && (line[0] == "location" || line[0] == "_"))
								break;
						}
					}
					cp++;
				}
			}
		}
	}
	return locationData;

}

std::vector < std::string >	Parsing::getLocationSingle(size_t serverIndex, size_t locationIndex, std::string data)
{
	std::vector < std::pair <std::string, std::vector < std::string > > > locationData (getLocationData(serverIndex, locationIndex));

	for (size_t i = 0; i < locationData.size(); i++)
	{
		if (locationData[i].first == data)
			return locationData[i].second;
	}
	return std::vector<std::string>();
}

std::vector < std::pair <std::string, std::string > > Parsing::getLocationMultiple(size_t serverIndex, size_t locationIndex, std::string dataToSearch)
{
	std::vector < std::pair <std::string, std::vector < std::string > > > locationData = getLocationData(serverIndex, locationIndex);
	std::vector < std::pair <std::string, std::string > > data;
	
	for (size_t i = 0; i < locationData.size(); i++)
	{
		if (locationData[i].first == dataToSearch)
		{
        	data.push_back(std::make_pair(locationData[i].second[0], locationData[i].second[1]));
		}
	}
	return data;
}



std::vector < std::pair <std::string, std::vector < std::string > > >	Parsing::serverData(size_t index)
{
	std::vector < std::pair <std::string, std::vector < std::string > > > serverData;
	if (index < 1 || index > servers.size())
		return serverData;
	
	for (size_t i = 0; i < this->servers.size(); i++)
	{
		if (i == index - 1)
		{
			for (size_t j = 0; j < servers[i].size(); j++)
			{
				std::vector < std::string > line = ft_split(servers[i][j]);
				if (line[0] == "location")
				{
					while (j < servers[i].size() && servers[i][j] != "_")
						j++;
					--j;
					continue;
				}
				if (line[0] == "listen" || line[0] == "server_name" || line[0] == "autoindex" || line[0] == "upload_dir" || line[0] == "root" || line[0] == "client_body_max_size")
				{
					std::vector < std::string > values (line.begin() + 1, line.end());
					serverData.push_back(std::make_pair(line[0], values));
				}
			}
		}
	}
	return serverData;
}

std::vector < std::pair < std::string, std::string > > Parsing::getServerErrorPages(size_t index)
{
	std::vector <  std::pair < std::string, std::string > > errorPages;
	if (index < 1 || index > servers.size())
		return errorPages;
	for (size_t i = 0; i < this->servers.size(); i++)
	{
		if (i == index - 1)
		{
			for (size_t j = 0; j < servers[i].size(); j++)
			{
				std::vector < std::string > line = ft_split(servers[i][j]);
				if (line[0] == "location")
				{
					while (j < servers[i].size() && servers[i][j] != "_")
						j++;
					--j;
					continue;
				}
				if (line[0] == "error_page")
					errorPages.push_back(std::make_pair(line[1], line[2]));
			}
		}
	}
	return errorPages;
}

std::string Parsing::getServerDataSingle (size_t serverIndex, std::string data)
{
	if (serverIndex < 1 || serverIndex > servers.size())
		return "";
	for (size_t i = 0; i < this->servers.size(); i++)
	{
		if (i == serverIndex - 1)
		{
			for (size_t j = 0; j < servers[i].size(); j++)
			{
				std::vector < std::string > line = ft_split(servers[i][j]);

				if (line[0] == "location")
				{
					while (j < servers[i].size() && servers[i][j] != "_")
						j++;
					--j;
					continue;
				}
				if (line[0] == data)
				{
					if (line[0] == "listen" && line[1].find(':') == std::string::npos)
						return "0.0.0.0:" + line[1];
					return line[1];
				}
			}
		}
	}
	return "";
}

int Parsing::checkForLocation (size_t serverIndex , const std::string& s)
{
	if (serverIndex < 1 || serverIndex > servers.size())
		return -1;
	int cp = 0;
	for (size_t i = 0; i < this->servers.size(); i++)
	{
		if (i == serverIndex - 1)
		{
			for (size_t j = 0; j < servers[i].size(); j++)
			{
				std::vector < std::string > line = ft_split(servers[i][j]);
				if (line[0] == "location" )
				{
					cp++;
					if (line[1] == s)
						return cp;
				}
				line.clear();
			}
			
		}
	}
	return -1;
}

std::string Parsing::getMimeTypes(const std::string& s)
{
	std::map<std::string, std::vector<std::string> >::const_iterator it = responseTypes.begin();
	for (; it != responseTypes.end(); ++it)
	{
		const std::vector<std::string>& mimeTypes = it->second;
		if (std::find(mimeTypes.begin(), mimeTypes.end(), s) != mimeTypes.end()) 
			return it->first;
    }   
    return "";
}


int	Parsing::getServerServerName(const std::string& key, const std::string& val)
{
	for (size_t i = 0; i < this->servers.size(); i++)
	{
		for (size_t j = 0; j < servers[i].size(); j++)
		{
			std::vector < std::string > line = ft_split(servers[i][j]);
			if (line[0] == key && line[1] == val)
				return line.clear(), i + 1;
			line.clear();
		}
	}
	return -1;
}

std::vector < std::pair < std::string, std::string > > Parsing::getAllPorts()
{
    std::vector < std::pair < std::string, std::string > > ports;
    std::vector < std::string > listen;
    std::vector < std::string > line;
    bool dupl;
    for (size_t i = 0; i < this->servers.size(); i++)
    {
        for (size_t j = 0; j < servers[i].size(); j++)
        {
            line = ft_split(servers[i][j]);
            if (line[0] == "listen")
            {
                dupl = false;
                for (size_t k = 0; k < listen.size(); k++)
                {
                    if (line[1] == listen[k])
                    {
                        dupl = true;
                        break;
                    }
                }
                if (!dupl)
                    listen.push_back(line[1]);
            }
            line.clear();
        }
    }

    for (size_t k = 0; k < listen.size(); k++)
    {
        line.clear();
        line = ft_split(listen[k], ':');
        ports.push_back(std::make_pair(line[0], line[1]));
    }
    line.clear();
    return ports;
}