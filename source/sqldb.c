#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libpq-fe.h>

#include "sqldb.h"

int
sqldb_init(struct sqldb_t *sql, const char *conn_str)
{
    PGconn *pgconn = PQconnectdb(conn_str);
    if (PQstatus(pgconn) != CONNECTION_OK) {
        fprintf(stderr, "Connection to database failed: %s\n", PQerrorMessage(pgconn));
        return (-1);
    }
    printf("Connected to database successfully\n");
    sql->db = pgconn;
    return (0);
}

void
sqldb_res_cleanup(char ***result, int rows, int cols)
{
    for (int i = 0; i < rows * cols; i++) {
        if (result[i] != NULL) free(result[i]);
    }
    
    if (result != NULL) free(result);  
}

void
sqldb_close(struct sqldb_t *sql)
{
    if (sql->db) {
        PQfinish((PGconn *)sql->db);
    }
    printf("Connection to database closed\n");
}

int
sqldb_query(struct sqldb_t *sql, const char *query, 
            char ***result, int *rows, int *cols)
{
    PGconn *pgconn = (PGconn *)sql->db;
    PGresult *res = PQexec(pgconn, query);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        if (PQresultStatus(res) == PGRES_COMMAND_OK) {
            return (0);
        }
        fprintf(stderr, "Query failed: %s %s\n", PQerrorMessage(pgconn), query);
        PQclear(res);
        return (-1);
    }

    if (rows != NULL) *rows = PQntuples(res);
    if (cols != NULL) *cols = PQnfields(res);

    if (rows != NULL && cols != NULL) {
        *result = malloc(*rows * *cols * sizeof(char *));
        
        for (int i = 0; i < *rows; i++) {
            for (int j = 0; j < *cols; j++) {
                (*result)[i * (*cols) + j] = strdup(PQgetvalue(res, i, j));
            }
        }
    }

    PQclear(res);
    return (0);
}
