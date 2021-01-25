cc_binary(
    name = "01_signal_trigger_timer",
    srcs = ["01_signal_trigger_timer.cpp"],
    deps = ["//:libevent"],
)

cc_binary(
    name = "02_create_multiple_connections",
    srcs = ["02_create_multiple_connections.cpp"],
    deps = [
        ":utility",
        "//:libevent",
    ],
)

cc_binary(
    name = "03_send_request_on_multiple_connections",
    srcs = ["03_send_request_on_multiple_connections.cpp"],
    #copts = [
    #    "-DDST_DEV_DOCKER",
    #],
    deps = [
        ":utility",
        "//:libevent",
    ],
)

cc_binary(
    name = "04_1_blocking_hostname_resolving",
    srcs = ["04_blocking_hostname_resolving.cpp"],
    deps = [
        ":utility",
        "//:libevent",
    ],
)

cc_binary(
    name = "04_2_portable_blocking_hostname_resolving",
    srcs = ["04_blocking_hostname_resolving.cpp"],
    copts = [
        "-DUSING_LIBEVENT",
    ],
    deps = [
        ":utility",
        "//:libevent",
    ],
)

cc_binary(
    name = "05_non_blocking_hostname_resolving",
    srcs = ["05_non_blocking_hostname_resolving.cpp"],
    deps = [
        ":utility",
        "//:libevent",
    ],
)

cc_library(
    name = "utility",
    srcs = ["utility.h"],
)
