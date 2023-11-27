#include "../../../includes/Request.hpp"

int     Request::Upload()
{
    
    return(0);
}

int     Request::CheckRessource()
{
	struct stat fileStat;

    if (stat(RequestPath.c_str(), &fileStat) == 0)
	{
		if (S_ISDIR(fileStat.st_mode))
			IsDirectory = true;
		else if (S_ISREG(fileStat.st_mode))
			IsDirectory = false;
		else
			return(statusCode = 404, 1);
	}
    else
		return(statusCode = 404, 1);

    return(0);
}

int     Request::GetRessource()
{
    if(IsDirectory == true && (URI[URI.size() - 1] != '/' || URI != "/"))
		return(statusCode = 301, 1);
    if(URI[URI.size() - 1] == '/' || URI == "/")
	{
		// std::cout << "POST DIR" << std::endl;
		if(GetDirectory())
			return(1);
	}
	else
	{
		// std::cout << "POST File" << std::endl;
		File = URI;
		if(GetFile())
			return(1);
	}

    return(0);
}

int     Request::PostFile()
{
    std::vector < std::pair <std::string, std::string > > srv = Server.getLocationMultiple(ServerIndex, locationIndex, "cgi");
    if(srv.size())
    {
        //Run CGI on requested file
        // std::vector < std::pair <std::string, std::string > >::iterator srvIT = srv.begin();
    }
    else
        return(statusCode = 403, 1);
    
    return(0);
}

int     Request::PostDir()
{
    std::vector < std::string> it;
	if(locationIndex != 0)
		it = Server.getLocationSingle(ServerIndex, locationIndex, "index");
	if(it.size())//If index files are present
	{
		File = it[0];
		if(PostFile())
			return(1);
	}
	else
        return(statusCode = 403, 1);

    return(0);
}

int		Request::POST()
{
    if(locationIndex != 0)//LOCATION
    {
        std::vector < std::string >	vec =  Server.getLocationSingle(ServerIndex, locationIndex, "upload_dir");
        if(vec.size())//IF SUPPORT UPLOAD
        {
            UploadDir = *vec.begin();
            if(Upload())
                return(1);
        }
        else
        {
            std::vector < std::string> it = Server.getLocationSingle(ServerIndex, locationIndex, "root");
            Loc.root = it[0];
            if(URI != "/")
                RequestPath = Loc.root + URI;
            else
                RequestPath = Loc.root;
            if(CheckRessource())
                    return(1);
        }
    }
    else//SERVER
    {
        std::string str =  Server.getServerDataSingle(ServerIndex, "upload_dir");
        if(str.size())//IF SUPPORT UPLOAD
        {
            UploadDir = str;
            if(Upload())
                return(1);
        }
        else
        {
		    RequestPath = Server.getServerDataSingle(ServerIndex, "root") + URI;
            if(CheckRessource())
                return(1);
        }
    }
	return(0);
}
