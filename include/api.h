#ifndef API_H
#define API_H

int is_auth(void *sql, const char *session_id);
char *api_auth(void *sql, const char *session_id);
char *api_login(void *sql, const char *data);
char *api_update(void *sql, const char *data, const char *session_id);
char *api_payment(void *sql, const char *data, const char *session_id);
char *api_reserve(void *sql, const char *data, const char *session_id);
char *api_reserves(void *sql, const char* session_id);
char *api_cancel(void *sql, const char *data, const char *session_id);
char *api_logout(void *sql, const char *session_id);
char *api_userinfo(void *sql, const char *session_id);
char *api_random(void);
char *api_register(void *sql, const char *data);
char *api_schedule(void *sql);

#endif