#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include "Config.hpp"
#include "FileHandler.hpp"
#include "Request.hpp"
#include "Server.hpp"
#include "Socket.hpp"
#include "utilities.hpp"

#include <arpa/inet.h>
#include <cstring>
#include <fstream>
#include <netinet/in.h>
#include <poll.h>
#include <sstream>
#include <vector>

const int CHUNK_SIZE = 64;

class ClientRequest
{
    // int                      fd; // pollfd client socket
    std::vector<Request>     reqs;
    std::vector<FileHandler> filehandlers;
};

class Webserv
{
  private:
    std::vector<Server>        servers;
    std::vector<struct pollfd> pfds;
    std::vector<FileHandler>   files; // Can be in ClientRequest
    std::map<int, std::string> res_status;
    std::vector<ClientRequest> clients;

    typedef struct Response
    {
        std::string content;
        std::string content_type;
        int         code;
    } Response;

    std::string recv_data;

    // Polling
    void poll_events();
    bool is_server_socket(int socket_fd);
    void accept_connection(int server_fd);
    void close_connection(int bytes_received, int client_index);

    // Handling request
    void        request_handler(int socket_fd);
    std::string handle_cgi(Config const& config, Request const& request,
                           int client_fd);
    std::string handle_uri(Config const& config, Request const& req,
                           Response& res);
    int         file_to_string(const char* path, std::string& string_buffer);
    void        respond(int socket_fd, Request& req, Response& res);

    // Utilities
    Server& get_server_from_client(int client_fd);
    Server& get_server(int server_fd);
    int     get_poll_index(int file_descriptor);

    std::vector<FileHandler>::iterator is_file_fd(int file_descriptor);
    std::vector<FileHandler>::iterator get_file_from_client(int dest_fd);

  public:
    Webserv();
    ~Webserv();

    void start();
    void create_server(Config& config);

    // Files handling
    static FileHandler open_file_stream(std::string filename,
                                        std::string mode = "r");
    static FileHandler open_file_stream(int         file_descriptor,
                                        std::string mode = "r");
};

#endif