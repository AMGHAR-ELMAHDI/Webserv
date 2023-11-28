#include "../../../includes/Request.hpp"

void    Request::Reset()
{
    HeaderData.clear();
    header.clear();
    body.clear();
    line.clear();
    http.clear();
    httpVersion.clear();
    methode.clear();
    URI.clear();
    RequestPath.clear();
    ResponseHeaders.clear();
    ResponseBody.clear();
    index = 0;
    statusCode = 200;
    locationIndex = 0;
    IsDirectory = 0;
    File.clear();
    UploadDir.clear();
    PathToSaveFile.clear();
    ds.clear();
    ServerIndex = 1;
    bzero(&Loc, sizeof(LOCATION));
    bzero(&Req, sizeof(Response));
    SendFile = 0;
    FileSize = 0;
    offset = 0;
    ClientIsDone = false;
    RequestIsDone = false;
    PathToSaveFile = "/Users/eamghar/Desktop/send";
}

int     Request::InitRequest(std::string str, int fd, int in, Parsing &ps)
{
    Reset();
    Server = ps;
    (void)fd;

    ServerIndex = in;
    // std::cout << "index---->" << ServerIndex << std::endl;
    // std::cout << "------------------------------------------------------------------1" << std::endl;
    this->getRequest(str);
    GenerateResponse();
    // std::cout << "------------------------------------------------------------------2" << std::endl;
    std::cout << "StatusCode: " << statusCode << std::endl;
    // std::cout << "-----------------------HEADER-------------------" << std::endl;
	// std::cout << ResponseHeaders << std::endl;
	// std::cout << "-----------------------BODY-------------------" << std::endl;
	// std::cout << ResponseBody << std::endl;
	// std::cout << "-----------------------END OF BODY-------------------" << std::endl;
    return(0);
}
