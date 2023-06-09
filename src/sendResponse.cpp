#include "../headers/ConfigFile.hpp"

/*
There are many different types of errors that can occur when handling a GET request. Here are some examples:

400 Bad Request: The request is malformed and cannot be understood by the server.
401 Unauthorized: The client is not authenticated and cannot access the requested resource.
403 Forbidden: The client is authenticated, but does not have permission to access the requested resource.
404 Not Found: The requested resource could not be found on the server.
405 Method Not Allowed: The requested resource does not support the HTTP method used in the request (e.g. GET vs POST).
406 Not Acceptable: The client requested a content type that is not supported by the server.
500 Internal Server Error: The server encountered an unexpected error while processing the request.

These are just a few examples of the many possible errors that can occur during a GET request.
In general, it's a good idea to handle errors gracefully by returning an appropriate error status code
and a human-readable error message in the response body.*/

string getContentType(const string& path)
{
    if (path.rfind(".html") != string::npos)
        return ("text/html");

    else if (path.rfind(".css") != string::npos)
        return ("text/css");

    else if (path.rfind(".js") != string::npos)
        return ("text/javascript");

    else if (path.rfind(".png") != string::npos)
        return ("image/png");

    else if (path.rfind(".jpg") != string::npos)
        return ("image/jpg");

    else if (path.rfind(".jpeg") != string::npos)
        return ("image/jpeg");

    else if (path.rfind(".gif") != string::npos)
        return ("image/gif");

    else if (path.rfind(".svg") != string::npos)
        return ("image/svg+xml");

    else if (path.rfind(".pdf") != string::npos)
        return ("application/pdf");

    else if (path.rfind(".zip") != string::npos)
        return ("application/zip");

    else if (path.rfind(".gz") != string::npos)
        return ("application/gzip");

    else if (path.rfind(".mp3") != string::npos)
        return ("audio/mpeg");

    else if (path.rfind(".mp4") != string::npos)
        return ("video/mp4");

    else if (path.rfind(".mkv") != string::npos)
        return ("video/x-matroska");

    else if (path.rfind(".xml") != string::npos)
        return ("application/xml");

    else if (path.rfind(".json") != string::npos)
        return ("application/json");

    else if (path.rfind(".ico") != string::npos)
        return ("image/x-icon");

    else if (path.rfind(".txt") != string::npos)
        return ("text/plain");

    else
        return ("application/octet-stream");
}

static const string    getStatusCode(const int status_code)
{
    switch (status_code)
    {
        case 200:
            return (" OK");
        case 201:
            return (" Created");
        case 204:
            return (" No Content");
        case 301:
            return (" Moved Permanently");
        case 400:
            return (" Bad Request");
        case 404:
            return (" Page Not Found");
        case 405:
            return (" Method Not Allowed");
        case 408:
            return (" Request Timeout");
        case 500:
            return (" Internal Server Error");
        case 501:
            return (" Not Implemented");
        default:
            return (" Unknown Error");
    }
}

void    GenerateResponse(const string& content, const string& content_type, const int status_code, t_client& client, string redir)
{
    string response;
    stringstream ss;

    ss << "HTTP/1.1 " << status_code << getStatusCode(status_code) << "\r\n";
    if (redir.size())
        ss << "Location: " << redir << "\r\n";
    else if (!client.request["method"].compare("GET"))
    {
        ss << "Content-Type: " << content_type << "\r\n";
        ss << "Content-Length: " << content.size() << "\r\n";
    }
    else
        ss << "Content-Length: " << content.size() << "\r\n";

    ss << "Set-Cookie: " << "sessionID=" << client.sessionID << "\r\n";
    ss << "Connection: close\r\n\r\n";
    ss << content;
    client.response = ss.str();
}

int  getRightLocation(string req_path, Server server)
{
    string  loc_path;
    int temp = 0;
    int j = -1;
    int count = 0;

    for (size_t i = 0; i < server.getSize(); i++)
    {
        loc_path = server.getLocation(i)->getPath();
        if(!loc_path.compare(req_path))
            return i;
        if (loc_path.compare("/"))
            loc_path.append("/");

        for (size_t z = 0; z < req_path.size() && z < loc_path.size() ; z++)
        {
            if (req_path[z] == loc_path[z])
            {
                if (req_path[z] == '/')
                    count++;
            }
            else
            {
                count = 0;
                break;
            }
        }
        if(temp < count || (temp == 1 && loc_path.size() == 1))
        {
            temp = count;
            j = i;
        }
            count = 0;
    }
    return (j);
}

string  getRightRoot(Server server, int loc_pos)
{
    if (server.getLocation(loc_pos)->getRoot().size())
       return (server.getLocation(loc_pos)->getRoot());
    else if (server.getValue("root").size())
        return (server.getValue("root"));
    return ("");
}

string  getRightContent(int fd)
{
    int r = 1;
    char buffer[1024];
    string content;

    if (fd == -1)
        return ("");

    while (r != 0)
    {
        bzero(buffer, 1024);
        r = read(fd, buffer, 1023);
        if (r >= 0)
            content.append(buffer, r);
    }
    return (close(fd), content);
}

int checkIfMethodAllowed(vector<string> allowedMethod, string method)
{
    for (size_t i = 0; i < allowedMethod.size(); i++)
        if (!allowedMethod.at(i).compare(method))
            return 1;
    return 0;
}

void    makeResponse(t_client& client, Server server)
{
    string method;

    method = client.request["method"];
    if (checkIfMethodAllowed(server.getLocation(getRightLocation(client.request["path"].substr(0, client.request["path"].find('?')),
       server))->getAllowedMethod(), method))
    {
        if (method == "GET")
            GetMethod(client, server);
        else if (method == "POST")
            PostMethod(client, server);
        else if (method == "DELETE")
            DeleteMethod(client, server);
        else
            GenerateResponse(getRightContent(open(server.getValue("501_error").c_str(), O_RDONLY)), (string&)"text/html", 501, client, "");
    }
    else
       GenerateResponse(getRightContent(open(server.getValue("405_error").c_str(), O_RDONLY)), (string&)"text/html", 405, client, "");
}

void checkRedir(t_client& client, Server server)
{
        if (server.getLocation(getRightLocation(client.request["path"].substr(0, client.request["path"].find('?')), server))->get_return().size())
                GenerateResponse("", "", 301, client, server.getLocation(getRightLocation(client.request["path"].substr(0, client.request["path"].find('?')), server))->get_return());
}

Server getRightServer(vector<Server *> servers, t_client client)
{
    string host = client.request["host"];
    string port = client.request["port"];

    if (servers.size() == 1)
        return (*servers[0]);

    if(port.size())
    {
        for (size_t i = 0; i < servers.size(); i++)
        {
            for(size_t j = 0; j < servers[i]->get_listens().size(); j++)
                if(servers[i]->get_listens().at(j) == port)
                    return (*servers[i]);
        }
    }
    else
    {
        for (size_t i = 0; i < servers.size(); i++)
            if (servers[i]->getValue("server_name") == host)
                return (*servers[i]);
    }
    return *servers[0];
}

