#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <unistd.h>
#include <glib.h>
#include <errno.h>
#include <string.h>
#include <rpc/object.h>
#include <rpc/client.h>
int
main(int argc, const char *argv[])
{
    rpc_client_t client;
    rpc_connection_t conn;
    rpc_object_t result;
    rpc_call_t call;
    const char *buf;
    int64_t len;
    int64_t num;
    int cnt = 0;
    if (argc > 1)
        client = rpc_client_create(argv[1], 0);
    else
        client = rpc_client_create("tcp://127.0.0.1:5000", 0);
    if (client == NULL) {
        result = rpc_get_last_error();
        fprintf(stderr, "cannot connect: %s\n",
            rpc_error_get_message(result));
        return (1);
    }
    conn = rpc_client_get_connection(client);
    result = rpc_connection_call_simple(conn, "hello", "[s]", "world");
    printf("result = %s\n", rpc_string_get_string_ptr(result));
    rpc_release(result);
    result = rpc_connection_call_simple(conn, "hello", "[s]", "world");
    printf("result = %s\n", rpc_string_get_string_ptr(result));
    rpc_release(result);
    if (rpc_connection_has_credentials(conn)) {
        fprintf(stderr, "Remote pid is %d\n",
            (int)rpc_connection_get_remote_pid(conn));
    }
        call = rpc_connection_call(conn, NULL, NULL, "stream", rpc_array_create(), NULL);
        if (call == NULL) {
                fprintf(stderr, "Stream call failed\n");
                rpc_client_close(client);
                return (1);
        }
        rpc_call_set_prefetch(call, 10);
        for (;;) {
                rpc_call_wait(call);
                switch (rpc_call_status(call)) {
        case RPC_CALL_STREAM_START:
            rpc_call_continue(call, false);
            break;
        case RPC_CALL_MORE_AVAILABLE:
            result = rpc_call_result(call);
            rpc_object_unpack(result, "[s, i, i]",
                &buf, &len, &num);
            cnt++;
            fprintf(stderr,
                "frag = %s, len = %" PRId64 ", num = %" PRId64 ","
                "cnt = %d\n", buf, len, num, cnt);
            g_assert(len == (int)strlen(buf));
            rpc_call_continue(call, false);
            break;
        case RPC_CALL_DONE:
        case RPC_CALL_ENDED:
            fprintf(stderr, "ENDED at %d\n", cnt);
            goto done;
        case RPC_CALL_ERROR:
            fprintf(stderr, "ERRORED out\n");
            goto done;
        case RPC_CALL_ABORTED:
            fprintf(stderr, "ABORTED at %d\n", cnt);
            goto done;
        default:
            break;
                }
        }
done:
        fprintf(stderr, "CLOSING client conn %p, cnt = %d\n", conn, cnt);
    rpc_client_close(client);
    return (0);
}
