#ifndef SERVER_H
#define SERVER_H

#define SHUTDOWN_SOFT 0
#define SHUTDONW_HARD 2

#define CHUNK_SIZE 4096

struct server_config_t {
    short port;
    int backlog;
    char *db_conn;
};

struct server_t {
    int socket;
    int epoll;
    struct sockaddr_in *address;
    struct server_config_t config;
    void *db;
};

void server_accept(struct server_t *server);
void handle_events(struct server_t *server, void *db, int *flag);

void server_cleanup(struct server_t *server);
int server_init(struct server_t *server, struct server_config_t *config, void *sql);
void server_shutdown(struct server_t *server, int mode);

void config_cleanup(struct server_config_t *config);
int config_load(const char *filename, struct server_config_t *config);

#endif
