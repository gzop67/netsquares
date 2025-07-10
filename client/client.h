#include "../defines.h"

bool8 client_connect();
void poll_server(void *);
bool8 send_to_server(char* dat, u32 size);
