#ifndef SQLDB_H
#define SQLDB_H

struct sqldb_t {
    void *db;
};

int sqldb_init(struct sqldb_t *sql, const char *line);
void sqldb_res_cleanup(char ***result, int rows, int cols);
void sqldb_close(struct sqldb_t *sql);
int sqldb_query(struct sqldb_t *sql, const char *query,
                char ***result, int *rows, int *cols);

#endif
