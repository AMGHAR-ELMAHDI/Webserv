#include "../../includes/Server.hpp"

int Server::nbr_srv = 0;

Server::Server(int port, std::string ip)
{
    res = NULL;
    this->ip = ip;
    this->port = port;
};

Server::~Server()
{
    close(serversocket);
}

void Server::start()
{
    int ret;

    nbr_srv++;
    index = 0;
    serverName = "server" + toString(nbr_srv);

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    ret = getaddrinfo(this->ip.c_str(), toString(port).c_str(), &hints, &res);
    if (ret != 0)
    {
        std::cerr << "Error: " << gai_strerror(ret) << std::endl;
        freeaddrinfo(res);
        exit(1);
    }
    this->serversocket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    this->fd = this->serversocket;
    if (serversocket == -1)
    {
        freeaddrinfo(res);
        std::cout << "ERROR IN SOCKET CREATION" << std::endl;
        exit(1);
    }
    std::cout << "[+] Server socket created" << std::endl;
    int opt = 1;
    setsockopt(serversocket, SOL_SOCKET, SO_REUSEADDR, (void *)&opt, sizeof(opt));
    if (bind(serversocket, res->ai_addr, res->ai_addrlen) == -1)
    {
        freeaddrinfo(res);
        std::cout << "FAILED TO BIND" << std::endl;
        close(serversocket);
        exit(1);
    }
    std::cout << "[+] Bound to port number: " << this->port << std::endl;
    freeaddrinfo(res);
    fcntl(serversocket, F_SETFL, O_NONBLOCK);
    if (listen(serversocket, 1000) == -1)
    {
        std::cout << "FAILED TO LISTEN" << std::endl;
        exit(1);
    }
    fd = serversocket;
    index = nbr_srv;
}

int IoMultiplexing::maxfd = 0;
std::vector<struct pollfd> IoMultiplexing::net;

int WaitForFullRequest(std::string& buff)
{
    std::string substringToFind = "\r\n\r\n";

    size_t found = buff.find(substringToFind);

    if (found != std::string::npos)
    {
        std::string STR = buff.substr(0, found);

        size_t index = buff.find('\n');
        if (index == std::string::npos)
            return (0);

        std::string http = buff.substr(0, index - 1);
        std::vector<std::string> vec = ft_split(http, " \n\r\t");

        if(vec.size() == 3)
        {
            if (*vec.begin() != "POST")
                return 1;
            else if (*vec.begin() == "POST")
            {
                size_t second = buff.find("Content-Length:");
                if (second != std::string::npos)
                {
                    second += 15;
                    size_t end = buff.find('\n', second);
                    if (end != std::string::npos)
                    {
                        std::string val = buff.substr(second, end - second);
                        int contentLength = std::atoi(val.c_str());

                        if (buff.size() >= found + substringToFind.size() + contentLength)
                            return 1;
                        else
                            return 0;
                    }
                }
                else if (buff.find("Transfer-Encoding: chunked") != std::string::npos)
                {
                    if (buff.find("0\r\n\r\n", found) != std::string::npos)
                        return 1;
                    else
                        return 0;
                }
                else if (buff.find("Content-Type: multipart/form-data") != std::string::npos)
                {
                    size_t boundaryStart = buff.find("boundary=");
                    if (boundaryStart != std::string::npos)
                    {
                        boundaryStart += 9;
                        size_t boundaryEnd = buff.find('\n', boundaryStart);
                        if (boundaryEnd != std::string::npos)
                        {
                            std::string boundary = buff.substr(boundaryStart, boundaryEnd - boundaryStart - 1);
                            std::string endBoundary = "--" + boundary + "--";
                            size_t endBoundaryPos = buff.find(endBoundary, found);
                            if (endBoundaryPos != std::string::npos)
                            {
                                size_t nextBoundaryPos = buff.find("--" + boundary, found);
                                if (nextBoundaryPos == std::string::npos || nextBoundaryPos >= endBoundaryPos)
                                    return 1;
                            }
                        }
                    }
                }
            }
        }
        else
            return 0;
    }
    return 0;
}

bool found_it(int fd, std::vector<class Server> &vec_serve)
{
    for(size_t y = 0; y < vec_serve.size(); y++)
        if (vec_serve[y].fd == fd)
            return true;
    return false;
}

void    IoMultiplexing::clearClinet(int fd, std::map<int, Client> &request_msg)
{
    std::map<int, Client>::iterator it = request_msg.find(fd);
    if (it != request_msg.end())
    {
        bool keepAlive = it->second.keepAlive;

        it->second.c_request.clear();
        it->second.c_response.clear();
        it->second.path.clear();
        it->second.initialPosition = 0;
        it->second.bytesRead = 0;
        it->second.currentPosition = 0;
        it->second.send_file = 0;
        it->second.keepAlive = 0;
        it->second.header = 0;

        it->second.keepAlive = keepAlive;
    }
}

int IoMultiplexing::StartTheMatrix(Parsing &ps)
{
    IoMultiplexing re;
    Request rq;

    rq.Server = ps;

    std::vector < std::pair < std::string, std::string > > test = ps.getAllPorts();
    std::vector < std::pair < std::string, std::string > >::iterator it = test.begin();
    for (; it != test.end(); it++)
    {
        Server qw(std::atoi(it->second.c_str()), it->first);
        re.sudo_apt.push_back(qw);
    }

    char buffer[50000];
    size_t ll = re.sudo_apt.size();
    for (size_t i = 0; i < ll; i++)
    {
        re.sudo_apt[i].start();
        struct pollfd tmp;
        tmp.fd = re.sudo_apt[i].serversocket;
        tmp.events = POLLIN;
        net.push_back(tmp);
    }
    std::cout << "[+] Listening...." << std::endl;
    std::string sttrr;
    signal(SIGPIPE, SIG_IGN);
    struct pollfd tmp;
    while (true)
    {
        struct timeval timeout;
        timeout.tv_sec = 30; 
        timeout.tv_usec = 1;
        int timeout_ms = timeout.tv_sec * 1000 + timeout.tv_usec / 1000;
        int ret = poll(&net[0], net.size(), timeout_ms);
        if (ret == -1)
            continue;
        if (ret == 0 || net.size() >= 10000)
        {
            for (size_t yt = re.sudo_apt.size(); yt < net.size(); yt++)
                close(net[yt].fd);
            net.clear();
            for (size_t i = 0; i < re.sudo_apt.size(); i++) 
            {
                tmp.fd = re.sudo_apt[i].fd;
                tmp.events = POLLIN;
                net.push_back(tmp);
            }
        }
        for (size_t j = 0; j < net.size(); j++)
        {
            if (net[j].revents & POLLIN)
            {
                if (found_it(net[j].fd, re.sudo_apt) && net[j].revents & POLLIN)
                {
                    int clientSocket = accept(net[j].fd, NULL, 0);
                    if (clientSocket == -1)
                        break;
                    struct pollfd tmps;
                    tmps.fd = clientSocket;
                    tmps.events = POLLIN;
                    fcntl(clientSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
                    IoMultiplexing::net.push_back(tmps);
                    continue;
                }
                bzero(buffer, 50000);
                int tt = recv(net[j].fd, buffer, 50000, 0);
                if (tt == 0)
                {
                    close(net[j].fd);
                    continue;
                }
                if (tt == -1)
                    continue;
                else
                {
                    std::string bu(buffer, tt);
                    re.request_msg[net[j].fd].c_request += bu;
                    bu.clear();
                    if ((WaitForFullRequest(re.request_msg[net[j].fd].c_request) == 1))
                    {
                        re.request_msg[net[j].fd].c_response = rq.InitRequest(re.request_msg[net[j].fd].c_request, ps);
                        re.request_msg[net[j].fd].c_request.clear();

                        re.request_msg[net[j].fd].send_file = rq.SendFile;
                        re.request_msg[net[j].fd].keepAlive = rq.KeepAlive;
                        re.request_msg[net[j].fd].path = rq.RequestPath;
                        re.request_msg[net[j].fd].header = false;
                        net[j].events = POLLOUT;
                    }
                    continue;
                }
            }
           else if (net[j].revents & POLLOUT)
           {
                if (re.request_msg[net[j].fd].send_file == false)
                {
                    size_t x_size = send(net[j].fd, re.request_msg[net[j].fd].c_response.c_str(), std::min((size_t) 1000000, re.request_msg[net[j].fd].c_response.length()), 0);
                    re.request_msg[net[j].fd].c_response.erase(0, x_size);
                    if (re.request_msg[net[j].fd].c_response.size() == 0)
                    {
                        net[j].events = POLLIN;
                        if(!re.request_msg[net[j].fd].keepAlive)
                            close(net[j].fd);
                    }
                    continue;
                }
                else
                {
                    if (re.request_msg[net[j].fd].header == false)
                    {
                        std::string& response = re.request_msg[net[j].fd].c_response;
                        ssize_t bytes_sent = send(net[j].fd, response.c_str(), response.length(), 0);
                        response.clear();

                        if (bytes_sent == -1 || bytes_sent == 0)
                            continue;

                        re.request_msg[net[j].fd].header = true;

                        re.request_msg[net[j].fd].fd = open(re.request_msg[net[j].fd].path.c_str(), O_RDONLY);
                        if (re.request_msg[net[j].fd].fd == -1)
                            continue;
                    }

                    char buffer[1024];

                    ssize_t bytes_read = read(re.request_msg[net[j].fd].fd, buffer, sizeof(buffer));
                    if (bytes_read == -1 || bytes_read == 0)
                    {
                        close(re.request_msg[net[j].fd].fd);
                        continue;
                    }

                    ssize_t bytes_sent = send(net[j].fd, buffer, bytes_read, 0);
                    if (bytes_sent == -1 || bytes_sent == 0 || bytes_sent < bytes_read)
                    {
                        if (!re.request_msg[net[j].fd].keepAlive)
                            close(net[j].fd);
                        clearClinet(net[j].fd, re.request_msg);
                        close(re.request_msg[net[j].fd].fd);
                        net[j].events = POLLIN;
                        continue;
                    }

                    bzero(buffer, sizeof(buffer));
                }
                continue;
            }
        }
    }
}

