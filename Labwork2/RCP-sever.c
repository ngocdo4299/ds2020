#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <glib.h>
#include <rpc/object.h>
#include <rpc/service.h>
#include <rpc/server.h>
#ifndef __unused
#define __unused __attribute__((unused))
#endif
static void
server_event(void *arg __unused, rpc_connection_t conn,
    rpc_server_event_t event)
{
    const char *addr;
    addr = rpc_connection_get_remote_address(conn);
    if (event == RPC_SERVER_CLIENT_CONNECT)
        printf("client %s connected\n", addr);
    if (event == RPC_SERVER_CLIENT_DISCONNECT)
        printf("client %s disconnected\n", addr);
}
static rpc_object_t
hello(void *cookie __unused, rpc_object_t args)
{
    (void)cookie;
    return rpc_string_create_with_format("hello %s!",
        rpc_array_get_string(args, 0));
}
int
main(int argc, const char *argv[])
{
    rpc_context_t ctx;
    rpc_object_t error;
    __block rpc_server_t srv;
        __block GRand *rand = g_rand_new();
        __block gint setcnt = g_rand_int_range(rand, 50, 500);
        __block char *strg = g_malloc(27);
    ctx = rpc_context_create();
    rpc_context_register_func(ctx, NULL, "hello",
        NULL, hello);
    rpc_context_register_block(ctx, NULL, "block",
        NULL, ^(void *cookie __unused, rpc_object_t args __unused) {
        return (rpc_string_create("haha lol"));
        });
    rpc_context_register_block(ctx, NULL, "delay",
        NULL, ^(void *cookie __unused, rpc_object_t args __unused) {
        sleep(5);
        return (rpc_int64_create(42));
        });
    rpc_context_register_block(ctx, NULL, "event",
        NULL, ^(void *cookie __unused, rpc_object_t args __unused) {
        rpc_server_broadcast_event(srv, NULL, NULL, "server.hello",
            rpc_string_create("world"));
        rpc_server_broadcast_event(srv, NULL, NULL, "oh_noes",
            rpc_int64_create(-1));
        return (rpc_null_create());
        });
        strcpy(strg, "abcdefghijklmnopqrstuvwxyz");
        rpc_context_register_block(ctx, NULL, "stream",
            NULL, ^rpc_object_t (void *cookie, rpc_object_t args __unused) {
                int cnt = 0;
                gint i;
                rpc_object_t res;
                rpc_function_start_stream(cookie);
                while (cnt < setcnt) {
                        cnt++;
                        i = g_rand_int_range (rand, 0, 26);
                        res = rpc_object_pack("[s, i, i]",
                            strg + i, (int64_t)(26-i), (int64_t)cnt);
                        fprintf(stderr, "returning %s,  %d letters, %d of %d\n",
                            rpc_array_get_string(res, 0), 26-i, cnt, setcnt);
                        if (rpc_function_yield(cookie, res) != 0) {
                                fprintf(stderr, "yield failed\n");
                                rpc_function_end(cookie);
                                return (rpc_null_create());
                        }
                }
                rpc_function_end(cookie);
        return (rpc_null_create());
        });
    if (argc > 1)
        srv = rpc_server_create(argv[1], ctx);
    else
        srv = rpc_server_create("tcp://0.0.0.0:5000", ctx);
    if (srv == NULL) {
        error = rpc_get_last_error();
        fprintf(stderr, "Cannot create server: %s\n",
            rpc_error_get_message(error));
        return (1);
    }
    rpc_server_set_event_handler(srv, RPC_SERVER_HANDLER(server_event, NULL));
    sleep(2);
    printf("Resuming server now\n");
    rpc_server_resume(srv);
    pause();
    rpc_server_close(srv);
    return (0);
}