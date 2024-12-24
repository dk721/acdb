#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "init.h"
#include "server.h"
#include "sqldb.h"
#include "util.h"

int server_shutdown_flag = 0;

void signal_handle(int sig) {
    if (sig == SIGINT || sig == SIGTERM) {
        server_shutdown_flag = 1;
    }
}

int main(int argc, char *argv[]) {
    struct server_config_t config;
    struct server_t server;

    init_locale();

    if (init(argc, argv) != 0) {
        return EXIT_FAILURE;
    }

    if (config_load("server.ini", &config) != 0) {
        fprintf(stderr, "Failed to load configuration.\n");
        return EXIT_FAILURE;
    }

    printf("Loaded config:\n");
    printf("  DB Connection: %s\n", config.db_conn);
    printf("  Server Port: %d\n", config.port);
    printf("  Backlog: %d\n", config.backlog);

    struct sqldb_t sql;
    sqldb_init(&sql, config.db_conn);
    if (sql.db == NULL) {
        return EXIT_FAILURE;
    }

    if (server_init(&server, &config, &sql) != 0) {
        fprintf(stderr, "Failed to initialize the server.\n");
        sqldb_close(&sql);
        return EXIT_FAILURE;
    }

    signal(SIGINT, signal_handle);
    signal(SIGTERM, signal_handle);

    handle_events(&server, &sql, &server_shutdown_flag);

    server_shutdown(&server, SHUTDOWN_SOFT);
    server_cleanup(&server);
    config_cleanup(&config);
    sqldb_close(&sql);

    return EXIT_SUCCESS;
}
