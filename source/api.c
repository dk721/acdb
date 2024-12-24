#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include <time.h>

#include "api.h"
#include "http.h"
#include "sqldb.h"

int
is_auth(void *sql, const char *session_id)
{
    char query[512];
    snprintf(query, sizeof(query), 
        "SELECT username FROM sessions WHERE token = '%s' AND expiration > NOW()", 
        session_id);

    char **result;
    int rows, cols;

    if (sqldb_query(sql, query, &result, &rows, &cols) != 0 || rows == 0) {
        return (0);
    }

    return (1);
}

char *
api_auth(void *sql, const char *session_id)
{
    char query[512];

    snprintf(query, sizeof(query), 
        "SELECT username FROM sessions WHERE token = '%s' AND expiration > NOW()", 
        session_id);

    char **result;
    int rows, cols;

    struct HTTPresp resp = {
        .status_code = 200,
        .mime = "application/json",
        .body = NULL
    };

    if (sqldb_query(sql, query, &result, &rows, &cols) != 0 || rows == 0) {
        resp.status_code = 200;
        strcpy(resp.body, "{\"status\": \"unauthorized\"}");
        return http_resp_gen(resp);
    }

    char username[256];
    strncpy(username, result[0], sizeof(username) - 1);
    username[sizeof(username) - 1] = '\0';

    sqldb_res_cleanup(result, rows, cols);

    snprintf(query, sizeof(query), 
        "SELECT passenger_id FROM users WHERE username = '%s'", 
        username);

    if (sqldb_query(sql, query, &result, &rows, &cols) != 0 || rows == 0) {
        resp.status_code = 200;
        snprintf(resp.body, sizeof(resp.body), "{\"status\": \"authorized\", \"username\": \"%s\"}", username);
        return http_resp_gen(resp);
    }

    char passenger_id[256];
    strncpy(passenger_id, result[0], sizeof(passenger_id) - 1);
    passenger_id[sizeof(passenger_id) - 1] = '\0';

    sqldb_res_cleanup(result, rows, cols);

    char body[512];
    snprintf(body, sizeof(body), "{\"status\": \"authorized\", \"username\": \"%s\", \"pass\": \"%s\"}", username, passenger_id);
    strcpy(resp.body, body);

    return http_resp_gen(resp);
}

char *
api_userinfo(void *sql, const char *session_id)
{
    char query[512];
    struct HTTPresp resp = {
        .status_code = 200,
        .mime = "application/json",
        .body = NULL
    };

    snprintf(query, sizeof(query),
             "SELECT u.username, u.balance, u.passenger_id, p.first_name, p.last_name, p.passport_number "
             "FROM users u "
             "LEFT JOIN passengers p ON u.passenger_id = p.passenger_id "
             "JOIN sessions s ON s.username = u.username "
             "WHERE s.token = '%s' AND s.expiration > NOW()",
             session_id);

    char **result;
    int rows, cols;
    if (sqldb_query(sql, query, &result, &rows, &cols) != 0 || rows == 0) {
        strcpy(resp.body, "{\"status\": \"error\", \"message\": \"User not found or session expired\"}");
        return http_resp_gen(resp);
    }

    const char *username = result[0];
    const char *balance = result[1];
    const char *passenger_id = result[2];
    const char *first_name = result[3];
    const char *last_name = result[4];
    const char *passport_number = result[5];

    char body[1024];
    snprintf(body, sizeof(body),
             "{\"status\": \"success\", \"username\": \"%s\", \"balance\": \"%s\", "
             "\"passenger\": {\"id\": \"%s\", \"first_name\": \"%s\", \"last_name\": \"%s\", \"passport\": \"%s\"}}",
             username, balance, passenger_id ? passenger_id : "null",
             first_name ? first_name : "", last_name ? last_name : "",
             passport_number ? passport_number : "");

    strcpy(resp.body, body);

    sqldb_res_cleanup(result, cols, rows);

    return http_resp_gen(resp);
}

char *
api_payment(void *sql, const char *data, const char *session_id)
{
    char query[512];
    char amount[32];
    struct HTTPresp resp = {
        .status_code = 200,
        .mime = "application/json",
        .body = NULL
    };

    sscanf(data, "{\"amount\":\"%32[^\"]\"}", amount);

    if (amount <= 0) {
        strcpy(resp.body, "{\"status\": \"error\", \"message\": \"Invalid amount\"}");
        return http_resp_gen(resp);
    }

    snprintf(query, sizeof(query), 
        "UPDATE users SET balance = balance + %s WHERE username = (SELECT username FROM sessions WHERE token = '%s' AND expiration > NOW())", 
        amount, session_id);

    if (sqldb_query(sql, query, NULL, NULL, NULL) != 0) {
        strcpy(resp.body, "{\"status\": \"error\", \"message\": \"Payment failed\"}");
        return http_resp_gen(resp);
    }

    strcpy(resp.body, "{\"status\": \"success\"}");
    return http_resp_gen(resp);
}


char *
api_update(void *sql, const char *data, const char *session_id)
{
    char query[1024] = {0};
    char first_name[256] = {0};
    char last_name[256] = {0};
    char passport_number[256] = {0};


    struct HTTPresp resp = {
        .status_code = 200,
        .mime = "application/json",
        .length = "",
        .body = NULL
    };

    sscanf(data, "{\"first_name\":\"%255[^\"]\",\"last_name\":\"%255[^\"]\",\"passport_number\":\"%255[^\"]\"}", 
           first_name, last_name, passport_number);

    snprintf(query, sizeof(query),
             "SELECT COALESCE(passenger_id, 0) FROM users WHERE username = (SELECT username FROM sessions WHERE token = '%s' AND expiration > NOW())", 
             session_id);

    char **result;
    int rows, cols;
    if (sqldb_query(sql, query, &result, &rows, &cols) != 0 || rows == 0) {
        strcpy(resp.body, "{\"status\":\"error\",\"message\":\"Session not found or expired\"}");
        return http_resp_gen(resp);
    }

    char passenger_id[32];
    strcpy(passenger_id, result[0]);
    sqldb_res_cleanup(result, cols, rows);

    if (strcmp(passenger_id, "0") != 0) {
        snprintf(query, sizeof(query),
                 "UPDATE passengers SET first_name = '%s', last_name = '%s', passport_number = '%s' "
                 "WHERE passenger_id = '%s'", 
                 first_name, last_name, passport_number, passenger_id);
    } else {
        snprintf(query, sizeof(query),
                 "INSERT INTO passengers (first_name, last_name, passport_number) VALUES ('%s', '%s', '%s')", 
                 first_name, last_name, passport_number);

        if (sqldb_query(sql, query, NULL, NULL, NULL) != 0) {
            strcpy(resp.body, "{\"status\":\"error\",\"message\":\"Failed to add passenger\"}");
            return http_resp_gen(resp);
        }

        snprintf(query, sizeof(query),
                 "SELECT passenger_id FROM passengers WHERE first_name = '%s' AND last_name = '%s' AND passport_number = '%s'", 
                 first_name, last_name, passport_number);
        
        if (sqldb_query(sql, query, &result, &rows, &cols) != 0 || rows == 0) {
            strcpy(resp.body, "{\"status\":\"error\",\"message\":\"Failed to retrieve passenger_id\"}");
            return http_resp_gen(resp);
        }

        strcpy(passenger_id, result[0]);
        sqldb_res_cleanup(result, cols, rows);

        snprintf(query, sizeof(query),
                 "UPDATE users SET passenger_id = '%s' WHERE username = (SELECT username FROM sessions WHERE token = '%s' AND expiration > NOW())", 
                 passenger_id, session_id);
    }

    if (sqldb_query(sql, query, NULL, NULL, NULL) != 0) {
        strcpy(resp.body, "{\"status\":\"error\",\"message\":\"Failed to update passenger info\"}");
        return http_resp_gen(resp);
    }

    strcpy(resp.body, "{\"status\":\"success\"}");
    return http_resp_gen(resp);
}

char *
api_login(void *sql, const char *data)
{
    char username[256] = {0};
    char password[256] = {0};

    struct HTTPresp resp = {
        .status_code = 200,
        .mime = "application/json",
        .length = "",
        .headers = "",
        .cookie = "",
        .body = NULL
    };

    sscanf(data, "{\"username\":\"%255[^\"]\",\"password\":\"%255[^\"]\"}", username, password);
    
    char query[512];
    snprintf(query, sizeof(query), "SELECT password_hash FROM users WHERE username = '%s'", username);

    char **result;
    int rows, cols;
    if (sqldb_query(sql, query, &result, &rows, &cols) != 0 || rows == 0) {
        strcpy(resp.body, "{\"status\": \"user not found\"}");
        return http_resp_gen(resp);
    }

    char stored_password_hash[1024];
    strcpy(stored_password_hash, result[0]);
    sqldb_res_cleanup(result, rows, cols);

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256_ctx;
    SHA256_Init(&sha256_ctx);
    SHA256_Update(&sha256_ctx, password, strlen(password));
    SHA256_Final(hash, &sha256_ctx);

    char password_hash[SHA256_DIGEST_LENGTH * 2 + 1];
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        snprintf(password_hash + i * 2, 3, "%02x", hash[i]);
    }

    if (strcmp(stored_password_hash, password_hash) != 0) {
        strcpy(resp.body, "{\"status\": \"invalid password\"}");
        return http_resp_gen(resp);
    }

    char session_id[64];
    snprintf(session_id, sizeof(session_id), "%lx%lx", time(NULL), rand());

    snprintf(query, sizeof(query), "INSERT INTO sessions (username, token, expiration) VALUES ('%s', '%s', NOW() + INTERVAL '1 DAY')"
                                    "ON CONFLICT (username) DO UPDATE SET token = EXCLUDED.token, expiration = EXCLUDED.expiration;",
                                    username, session_id);
    if (sqldb_query(sql, query, NULL, NULL, NULL) != 0) {
        strcpy(resp.body, "{\"status\": \"session creation error\"}");
        return http_resp_gen(resp);
    }

    snprintf(resp.body, sizeof(resp.body), "{\"status\": \"logged in\"}");
    snprintf(resp.cookie, sizeof(resp.cookie), "session_id=%s; HttpOnly; Path=/;", session_id);


    return http_resp_gen(resp);
}

char *
api_register(void *sql, const char *data)
{
    char username[256] = {0};
    char email[256] = {0};
    char password[256] = {0};

    struct HTTPresp resp = {
        .status_code = 200,
        .mime = "application/json",
        .length = "",
        .headers = "",
        .cookie = "",
        .body = NULL
    };

    sscanf(data,
        "{\"username\":\"%255[^\"]\",\"email\":\"%255[^\"]\",\"password\":\"%255[^\"]\"}",
        username, email, password);
    
    char query[512];
    snprintf(query, sizeof(query), "SELECT 1 FROM users WHERE username = '%s' OR email = '%s'", username, email);

    char **result;
    int rows, cols;
    if (sqldb_query(sql, query, &result, &rows, &cols) == 0 && rows > 0) {
        strcpy(resp.body, "{\"status\": \"user exist\"}");
        return http_resp_gen(resp);
    }
    
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256_ctx;
    SHA256_Init(&sha256_ctx);
    SHA256_Update(&sha256_ctx, password, strlen(password));
    SHA256_Final(hash, &sha256_ctx);

    char password_hash[SHA256_DIGEST_LENGTH * 2 + 1];
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        snprintf(password_hash + i * 2, 3, "%02x", hash[i]);
    }

    snprintf(query, sizeof(query), "INSERT INTO users (username, email, password_hash) VALUES ('%s', '%s', '%s')", username, email, password_hash);
    if (sqldb_query(sql, query, NULL, NULL, NULL) != 0) {
        strcpy(resp.body, "{\"status\": \"register error\"}");
        return http_resp_gen(resp);
    }

    sqldb_res_cleanup(result, rows, cols);

    char session_id[64];
    snprintf(session_id, sizeof(session_id), "%lx%lx", time(NULL), rand());

    snprintf(query, sizeof(query), "INSERT INTO sessions (username, token, expiration) VALUES ('%s', '%s', NOW() + INTERVAL '1 DAY') "
                                    "ON CONFLICT (username) DO UPDATE SET token = EXCLUDED.token, expiration = EXCLUDED.expiration;",
                                    username, session_id);
    if (sqldb_query(sql, query, NULL, NULL, NULL) != 0) {
        strcpy(resp.body, "{\"status\": \"session creation error\"}");
        return http_resp_gen(resp);
    }

    snprintf(resp.body, sizeof(resp.body), "{\"status\": \"logged in\"}");
    snprintf(resp.cookie, sizeof(resp.cookie), "session_id=%s; HttpOnly; Path=/;", session_id);


    strcpy(resp.body, "{\"status\": \"registered\"}");
    return http_resp_gen(resp);
}

char *
api_logout(void *sql, const char *session_id)
{
    char query[512];
    snprintf(query, sizeof(query), "DELETE FROM sessions WHERE token = '%s'", session_id);

    struct HTTPresp resp = {
        .status_code = 200,
        .mime = "application/json",
        .length = "",
        .headers = "",
        .body = NULL
    };

    if (is_auth(sql, session_id) == 0) {
        sprintf(resp.body, "{\"status\": \"not authorized\"}");
        return http_resp_gen(resp);
    }

    if (sqldb_query(sql, query, NULL, NULL, NULL) != 0) {
        sprintf(resp.body, "{\"status\": \"error\"}");
        return http_resp_gen(resp);
    }

    sprintf(resp.body, "{\"status\": \"logged out\"}");
    return http_resp_gen(resp);
}


char *
api_random(void)
{
    struct HTTPresp resp = {
        .status_code = 200,
        .mime = "application/json",
        .length = "",
        .headers = "",
        .body = NULL
    };

    int random_number = rand() % 1000;
    char body[128];

    snprintf(body, sizeof(body), "{\"random_number\": %d}", random_number);
    strcpy(resp.body, body);

    snprintf(resp.length, sizeof(resp.length), "%lu", strlen(body));

    return http_resp_gen(resp);
}

char *
api_reserves(void *sql, const char* session_id)
{
    char query[512];
    struct HTTPresp resp = {
        .status_code = 200,
        .mime = "application/json",
        .length = "",
        .headers = "",
        .body = NULL
    };
    // Получаем passenger_id из session_id
    snprintf(query, sizeof(query),
             "SELECT passenger_id FROM users u JOIN sessions s ON u.username = s.username "
             "WHERE s.token = '%s' AND s.expiration > NOW()",
             session_id);

    char **result;
    int rows, cols;
    if (sqldb_query(sql, query, &result, &rows, &cols) != 0 || rows == 0) {
        strcpy(resp.body, "{\"status\": \"error\", \"message\": \"User not authorized\"}");
        return http_resp_gen(resp);
    }

    int passenger_id = atoi(result[0]);
    sqldb_res_cleanup(result, cols, rows);

    snprintf(query, sizeof(query),
             "SELECT flight_id FROM reserves WHERE passenger_id = %d",
             passenger_id);

    if (sqldb_query(sql, query, &result, &rows, &cols) != 0 || rows == 0) {
        strcpy(resp.body, "{\"status\": \"error\", \"message\": \"No reserved flights found\"}");
        return http_resp_gen(resp);
    }

    char reserved_flights[1024] = "[";
    for (int i = 0; i < rows; ++i) {
        if (i > 0) {
            strcat(reserved_flights, ", ");
        }
        strcat(reserved_flights, result[i]);
    }
    strcat(reserved_flights, "]");

    snprintf(resp.body, sizeof(resp.body), "{\"status\": \"success\", \"reserved_flights\": %s}", reserved_flights);
    sqldb_res_cleanup(result, cols, rows);
    return http_resp_gen(resp);
}

char *
api_reserve(void *sql, const char *data, const char *session_id)
{
    char query[512];
    int flight_id;

    struct HTTPresp resp = {
        .status_code = 200,
        .mime = "application/json",
        .body = NULL
    };

    // Извлечение flight_id из JSON
    sscanf(data, "{\"flight_id\":%d}", &flight_id);

    // Проверка flight_id
    if (flight_id <= 0) {
        strcpy(resp.body, "{\"status\": \"error\", \"message\": \"Invalid flight ID\"}");
        return http_resp_gen(resp);
    }

    // Получение passenger_id и текущего баланса по session_id
    snprintf(query, sizeof(query),
             "SELECT passenger_id, balance FROM users u JOIN sessions s ON u.username = s.username "
             "WHERE s.token = '%s' AND s.expiration > NOW()",
             session_id);

    char **result;
    int rows, cols;
    if (sqldb_query(sql, query, &result, &rows, &cols) != 0 || rows == 0) {
        strcpy(resp.body, "{\"status\": \"error\", \"message\": \"User not authorized or passenger not found\"}");
        return http_resp_gen(resp);
    }

    int passenger_id = atoi(result[0]);
    int current_balance = atoi(result[1]);
    sqldb_res_cleanup(result, cols, rows);

    // Получение стоимости рейса и проверка времени вылета
    snprintf(query, sizeof(query),
             "SELECT price FROM flights WHERE flight_id = %d AND departure_time > NOW() + INTERVAL '2 HOUR'",
             flight_id);

    if (sqldb_query(sql, query, &result, &rows, &cols) != 0 || rows == 0) {
        strcpy(resp.body, "{\"status\": \"error\", \"message\": \"Flight not found or departure time too soon\"}");
        return http_resp_gen(resp);
    }

    int flight_price = atoi(result[0]);
    sqldb_res_cleanup(result, cols, rows);

    // Проверка достаточности баланса
    if (current_balance < flight_price) {
        strcpy(resp.body, "{\"status\": \"error\", \"message\": \"Insufficient balance\"}");
        return http_resp_gen(resp);
    }

    // Проверка наличия бронирования
    snprintf(query, sizeof(query),
             "SELECT 1 FROM reserves WHERE passenger_id = %d AND flight_id = %d",
             passenger_id, flight_id);

    if (sqldb_query(sql, query, &result, &rows, &cols) == 0 && rows > 0) {
        sqldb_res_cleanup(result, cols, rows);
        strcpy(resp.body, "{\"status\": \"error\", \"message\": \"Flight already reserved\"}");
        return http_resp_gen(resp);
    }

    sqldb_res_cleanup(result, cols, rows);

    // Списание стоимости рейса с баланса
    snprintf(query, sizeof(query),
             "UPDATE users SET balance = balance - %d WHERE passenger_id = %d",
             flight_price, passenger_id);

    if (sqldb_query(sql, query, NULL, NULL, NULL) != 0) {
        strcpy(resp.body, "{\"status\": \"error\", \"message\": \"Failed to update balance\"}");
        return http_resp_gen(resp);
    }

    // Добавление бронирования
    snprintf(query, sizeof(query),
             "INSERT INTO reserves (passenger_id, flight_id) VALUES (%d, %d)",
             passenger_id, flight_id);

    if (sqldb_query(sql, query, NULL, NULL, NULL) != 0) {
        strcpy(resp.body, "{\"status\": \"error\", \"message\": \"Failed to reserve flight\"}");
        return http_resp_gen(resp);
    }

    strcpy(resp.body, "{\"status\": \"success\", \"message\": \"Flight reserved successfully\"}");
    return http_resp_gen(resp);
}

char *
api_cancel(void *sql, const char *data, const char *session_id)
{
    char query[512];
    int flight_id;

    struct HTTPresp resp = {
        .status_code = 200,
        .mime = "application/json",
        .body = NULL
    };

    // Извлечение flight_id из JSON
    sscanf(data, "{\"flight_id\":%d}", &flight_id);

    // Проверка flight_id
    if (flight_id <= 0) {
        strcpy(resp.body, "{\"status\": \"error\", \"message\": \"Invalid flight ID\"}");
        return http_resp_gen(resp);
    }

    // Получение passenger_id по session_id
    snprintf(query, sizeof(query),
             "SELECT passenger_id FROM users u JOIN sessions s ON u.username = s.username "
             "WHERE s.token = '%s' AND s.expiration > NOW()",
             session_id);

    char **result;
    int rows, cols;
    if (sqldb_query(sql, query, &result, &rows, &cols) != 0 || rows == 0) {
        strcpy(resp.body, "{\"status\": \"error\", \"message\": \"User not authorized or passenger not found\"}");
        return http_resp_gen(resp);
    }

    int passenger_id = atoi(result[0]);
    sqldb_res_cleanup(result, rows, cols);

    // Проверка времени вылета
    snprintf(query, sizeof(query),
             "SELECT departure_time FROM flights "
             "WHERE flight_id = %d AND departure_time > NOW() + INTERVAL '2 HOUR'",
             flight_id);

    if (sqldb_query(sql, query, &result, &rows, &cols) != 0 || rows == 0) {
        strcpy(resp.body, "{\"status\": \"error\", \"message\": \"Cannot cancel reservation after flight departure\"}");
        return http_resp_gen(resp);
    }

    sqldb_res_cleanup(result, rows, cols);

    // Удаление бронирования
    snprintf(query, sizeof(query),
             "DELETE FROM reserves WHERE passenger_id = %d AND flight_id = %d",
             passenger_id, flight_id);

    if (sqldb_query(sql, query, NULL, NULL, NULL) != 0) {
        strcpy(resp.body, "{\"status\": \"error\", \"message\": \"Failed to cancel reservation\"}");
        return http_resp_gen(resp);
    }

    strcpy(resp.body, "{\"status\": \"success\", \"message\": \"Reservation cancelled successfully\"}");
    return http_resp_gen(resp);
}


char *
api_schedule(void *sql)
{
    char **result;
    int rows, cols;
    const char query[] = "SELECT f.flight_id, f.departure_time, f.arrival_time, "
                        "da.name AS departure_airport, da.city AS departure_city, da.country AS departure_country, "
                        "aa.name AS arrival_airport, aa.city AS arrival_city, aa.country AS arrival_country, "
                        "f.price "
                        "FROM flights f "
                        "JOIN airports da ON f.departure_airport_id = da.airport_id "
                        "JOIN airports aa ON f.arrival_airport_id = aa.airport_id "
                        "WHERE f.departure_time > NOW() + INTERVAL '2 HOUR';";


    int status = sqldb_query(sql, query, &result, &rows, &cols);
    if (status == 0) {
        char json_body[2048];
        strcpy(json_body, "[");

        for (int i = 0; i < rows; i++) {
        char flight_json[512];
        snprintf(flight_json, sizeof(flight_json),
                 "{\"id\": %s, "
                 "\"departure_time\": \"%s\", "
                 "\"arrival_time\": \"%s\", "
                 "\"departure_airport\": \"%s\", "
                 "\"departure_city\": \"%s\", "
                 "\"departure_country\": \"%s\", "
                 "\"arrival_airport\": \"%s\", "
                 "\"arrival_city\": \"%s\", "
                 "\"arrival_country\": \"%s\", "
                 "\"price\": \"%s\"}",
                 result[i * cols + 0],
                 result[i * cols + 1],
                 result[i * cols + 2],
                 result[i * cols + 3],
                 result[i * cols + 4],
                 result[i * cols + 5],
                 result[i * cols + 6],
                 result[i * cols + 7],
                 result[i * cols + 8],
                 result[i * cols + 9]);

            strcat(json_body, flight_json);
            if (i < rows - 1) {
                strcat(json_body, ",");
            }
        }
        strcat(json_body, "]");

        struct HTTPresp resp = {
            .status_code = 200,
            .mime = "application/json",
            .length = "",
            .headers = "",
            .body = NULL
        };

        snprintf(resp.length, sizeof(resp.length), "%lu", strlen(json_body));
        strcpy(resp.body, json_body);

        char *response = http_resp_gen(resp);

        for (int i = 0; i < rows * cols; i++) {
            free(result[i]);
        }
        
        free(result);

        return response;
    } else {
        fprintf(stderr, "Failed to fetch flights\n");
        return NULL;
    }
}