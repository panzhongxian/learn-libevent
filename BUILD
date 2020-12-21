load("@rules_foreign_cc//tools/build_defs:configure.bzl", "configure_make")

configure_make(
    name = "openssl",
    configure_command = "config",
    configure_options = [
        "no-shared",
    ],
    lib_source = "@openssl//:all",
    linkopts = [
        "-ldl",
    ],
    static_libraries = [
        "libssl.a",
        "libcrypto.a",
    ],
    visibility = ["//visibility:public"],
)

configure_make(
    name = "libevent",
    configure_command = "configure",
    configure_env_vars = {
        "AR": "",
    },
    configure_options = [
        "--enable-shared=no",
        "--disable-libevent-regress",
        "LDFLAGS=\"$LDFLAGS -ldl -lssl -lcrypto\"",
    ],
    lib_source = "@libevent//:all",
    linkopts = [
        "-ldl",
    ],
    out_lib_dir = "lib",
    visibility = ["//visibility:public"],
    deps = ["openssl"],
)
