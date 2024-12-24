#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <libconfig.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/epoll.h>

#include "api.h"
#include "http.h"
#include "server.h"
#include "util.h"

struct server_epoll_event_t {
    struct epoll_event event;
    int fd;
};

static void _socket_nonblock(int socket);
static void _socket_reuseaddr(int socket);
static void _sendfile(int client_socket, const char *file_path);

void
handle_client(int client_socket, struct sqldb_t *db)
{
    char buffer[2048];
    ssize_t bytes_received;
    
    bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytes_received <= 0) {
        close(client_socket);
        return;
    } else
        buffer[bytes_received] = '\0';

    struct HTTPreq request = http_req_parse(buffer);
    loglvl("%s:%s:%s", request.path, request.method, request.cookie);

    if (strcmp(request.path, "/api/rand") == 0) {
        char *response = api_random();
        send(client_socket, response, strlen(response), 0);
    } else if (strcmp(request.path, "/api/schedule") == 0) {
        char *response = api_schedule(db);
        send(client_socket, response, strlen(response), 0);
    } else if (strcmp(request.path, "/api/reserves") == 0) {
        char *response = api_reserves(db, &request.cookie[11]);
        send(client_socket, response, strlen(response), 0);
    } else if (strcmp(request.path, "/api/register") == 0 &&
               strcmp(request.method, "POST") == 0) {
        char *response = api_register(db, &strstr(buffer, "\r\n\r\n")[4]);
        send(client_socket, response, strlen(response), 0);
    } else if (strcmp(request.path, "/api/login") == 0 &&
               strcmp(request.method, "POST") == 0) {
        char *response = api_login(db, &strstr(buffer, "\r\n\r\n")[4]);
        send(client_socket, response, strlen(response), 0);
    } else if (strcmp(request.path, "/api/update") == 0 &&
               strcmp(request.method, "POST") == 0) {
        char *response = api_update(db, &strstr(buffer, "\r\n\r\n")[4], &request.cookie[11]);
        send(client_socket, response, strlen(response), 0);
    } else if (strcmp(request.path, "/api/payment") == 0 &&
               strcmp(request.method, "POST") == 0) {
        char *response = api_payment(db, &strstr(buffer, "\r\n\r\n")[4], &request.cookie[11]);
        send(client_socket, response, strlen(response), 0);
    } else if (strcmp(request.path, "/api/reserve") == 0 &&
               strcmp(request.method, "POST") == 0) {
        char *response = api_reserve(db, &strstr(buffer, "\r\n\r\n")[4], &request.cookie[11]);
        send(client_socket, response, strlen(response), 0);
    } else if (strcmp(request.path, "/api/cancel") == 0 &&
               strcmp(request.method, "POST") == 0) {
        char *response = api_cancel(db, &strstr(buffer, "\r\n\r\n")[4], &request.cookie[11]);
        send(client_socket, response, strlen(response), 0);
    } else if (strcmp(request.path, "/api/auth") == 0) {
        char *response = api_auth(db, &request.cookie[11]);
        send(client_socket, response, strlen(response), 0);
    } else if (strcmp(request.path, "/api/logout") == 0) {
        char *response = api_logout(db, &request.cookie[11]);
        send(client_socket, response, strlen(response), 0);
    } else if (strcmp(request.path, "/api/userinfo") == 0) {
        char *response = api_userinfo(db, &request.cookie[11]);
        send(client_socket, response, strlen(response), 0);
    } else {
        _sendfile(client_socket, &request.path[1]);
    }

    close(client_socket);
}

int
create_epoll(struct server_t *server)
{
    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1");
        close(server->socket);
        exit(EXIT_FAILURE);
    }

    struct server_epoll_event_t server_event;
    server_event.event.events = EPOLLIN;
    server_event.event.data.fd = server->socket;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server->socket, &server_event.event) == -1) {
        perror("epoll_ctl: server socket");
        close(server->socket);
        close(epoll_fd);
        exit(EXIT_FAILURE);
    }

    return epoll_fd;
}

void
server_cleanup(struct server_t *server)
{
    if (server->address != NULL) {
        free(server->address);
    }
    if (server->socket != -1) {
        close(server->socket);
    }
    printf("Server cleaned up.\n");
}

int
server_init(struct server_t *server, struct server_config_t *config, void *sql)
{
    server->config = *config;
    server->db = sql;

    server->socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server->socket == -1) {
        perror("Socket creation failed:");
        return (-1);
    }

    _socket_reuseaddr(server->socket);
    _socket_nonblock(server->socket);

    server->address = malloc(sizeof(struct sockaddr_in));
    if (server->address == NULL) {
        perror("Memory allocation failed for address:");
        close(server->socket);
        return (-1);
    }

    memset(server->address, 0, sizeof(struct sockaddr_in));
    server->address->sin_family = AF_INET;
    server->address->sin_addr.s_addr = INADDR_ANY;
    server->address->sin_port = htons(config->port);

    if (bind(server->socket, (struct sockaddr *)server->address, sizeof(struct sockaddr_in)) == -1) {
        perror("Binding failed:");
        close(server->socket);
        free(server->address);
        return (-1);
    }

    if (listen(server->socket, config->backlog) == -1) {
        perror("Listen failed:");
        close(server->socket);
        free(server->address);
        return (-1);
    }

    (server->epoll) = create_epoll(server);

    printf("Server started on port %d\n", config->port);
    return (0);
}

void handle_events(struct server_t *server, void *db, int *flag) {
    struct epoll_event events[server->config.backlog];
    while (*flag == 0) {
        int num_events = epoll_wait(server->epoll, events, 10, 1000);
        if (num_events == -1) {
            perror("epoll_wait");
            break;
        }

        for (int i = 0; i < num_events; i++) {
            if (events[i].data.fd == server->socket) {
                struct sockaddr_in client_addr;
                socklen_t client_len = sizeof(client_addr);
                int client_fd = accept(server->socket, (struct sockaddr *)&client_addr, &client_len);
                if (client_fd == -1) {
                    perror("accept");
                    continue;
                }

                _socket_nonblock(client_fd);
                struct server_epoll_event_t client_event;
                client_event.event.events = EPOLLIN | EPOLLET;
                client_event.event.data.fd = client_fd;
                if (epoll_ctl(server->epoll, EPOLL_CTL_ADD, client_fd, &client_event.event) == -1) {
                    perror("epoll_ctl: client socket");
                    close(client_fd);
                }
            } else {
                handle_client(events[i].data.fd, (struct sqldb_t *)db);
            }
        }
    }
}


void
server_shutdown(struct server_t *server, int mode)
{
    switch (mode) {
    case SHUTDOWN_SOFT:
        shutdown(server->socket, SHUT_RD);
        printf("Shutting down server gracefully...\n");
        break;
    default:
        shutdown(server->socket, SHUT_RDWR);
        printf("Force shutdown server!\n");
    }
}

void
config_cleanup(struct server_config_t *config)
{
    if (config->db_conn != NULL) {
        free(config->db_conn);
    }
}

int
config_load(const char *filename, struct server_config_t *config)
{
    config_t cfg;
    config_setting_t *setting;
    
    config_init(&cfg);
    
    if (!config_read_file(&cfg, filename)) {
        fprintf(stderr, "Error reading config file %s\n", filename);
        config_destroy(&cfg);
        return (-1);
    }

    setting = config_lookup(&cfg, "server.port");
    if (setting != NULL) {
        config->port = config_setting_get_int(setting);
    } else {
        fprintf(stderr, "No 'server.port' setting found in config file\n");
        config_destroy(&cfg);
        return (-1);
    }

    setting = config_lookup(&cfg, "server.db_conn");
    if (setting != NULL) {
        config->db_conn = strdup(config_setting_get_string(setting));
    } else {
        fprintf(stderr, "No 'server.db_conn' setting found in config file\n");
        config_destroy(&cfg);
        return (-1);
    }

    if (config->db_conn[strlen(config->db_conn) - 1] == '\n') {
        config->db_conn[strlen(config->db_conn) - 1] = '\0';
    }

    setting = config_lookup(&cfg, "server.backlog");
    if (setting != NULL) {
        config->backlog = config_setting_get_int(setting);
    } else {
        fprintf(stderr, "'server.backlog' not find, setting by default: 16\n");
        config->backlog = 16;
    }

    config_destroy(&cfg);
    return (0);
}

static void
_sendfile(int client_socket, const char *file_path)
{
    struct stat file_stat;
    if (stat(file_path, &file_stat) != 0) {
        struct HTTPresp resp = {
            .status_code = 404,
            .mime = "text/html",
            .body = "<html><body><h1>404 Not Found</h1></body></html>"
        };
        char *response = http_resp_gen(resp);
        send(client_socket, response, strlen(response), 0);
        return;
    }

    FILE *file = fopen(file_path, "rb");
    if (file == NULL) {
        struct HTTPresp resp = {
            .status_code = 500,
            .mime = "text/html",
            .body = "<html><body><h1>500 Internal Server Error</h1></body></html>"
        };
        char *response = http_resp_gen(resp);
        send(client_socket, response, strlen(response), 0);
        return;
    }

    struct HTTPresp resp = {
        .status_code = 200,
        .mime = "",
        .length = "",
        .headers = "",
        .body = NULL
    };

    strncpy(resp.mime, http_mime(file_path), sizeof(resp.mime) - 1);
    snprintf(resp.length, sizeof(resp.length), "%ld", file_stat.st_size);

    char *response_headers = http_resp_gen(resp);
    send(client_socket, response_headers, strlen(response_headers), 0);

    char buffer[CHUNK_SIZE];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        if (send(client_socket, buffer, bytes_read, 0) < 0) {
            perror("Error sending file chunk");
            break;
        }
    }

    fclose(file);
}

static void
_socket_nonblock(int socket) {
    int flags = fcntl(socket, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl() failed:");
        return;
    }

    if (fcntl(socket, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl() failed to set non-blocking:");
        return;
    }
}

static void
_socket_reuseaddr(int socket) {
    int optval = 1;
    if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
        perror("setsockopt() failed for SO_REUSEADDR:");
        return;
    }
}