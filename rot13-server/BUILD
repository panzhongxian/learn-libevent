cc_library(
    name = "rot13-helper",
    srcs = ["helper.h"],
    deps = ["//:libevent"],
)

cc_binary(
    name = "http-client",
    srcs = ["stage_0.cpp"],
    deps = ["rot13-helper"],
)

cc_binary(
    name = "multi-process-server",
    srcs = ["stage_1.cpp"],
    deps = ["rot13-helper"],
)

cc_binary(
    name = "multi-thread-server",
    srcs = ["stage_2.cpp"],
    linkopts = ["-pthread"],
    deps = ["rot13-helper"],
)

cc_binary(
    name = "select-server",
    srcs = ["stage_3.cpp"],
    deps = ["rot13-helper"],
)

cc_binary(
    name = "epoll-server",
    srcs = ["stage_4.cpp"],
    deps = ["rot13-helper"],
)

cc_binary(
    name = "low-libevent-server",
    srcs = ["stage_5.cpp"],
    deps = ["rot13-helper"],
)

cc_binary(
    name = "high-libevent-server",
    srcs = ["stage_6.cpp"],
    deps = ["rot13-helper"],
)
