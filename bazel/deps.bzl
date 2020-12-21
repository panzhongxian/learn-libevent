load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

def msdk_toolbox_deps():
    #maybe(
    #    http_archive,
    #    name = "rules_foreign_cc",
    #    strip_prefix = "rules_foreign_cc-master",
    #    url = "https://github.com/bazelbuild/rules_foreign_cc/archive/master.zip",
    #)

    maybe(
        git_repository,
        name = "rules_foreign_cc",
        remote = "https://github.com/whs-dot-hk/whs_rules_foreign_cc.git",
        branch = "fix-pkgconfig",
    )

    all_content = """filegroup(name = "all", srcs = glob(["**"]), visibility = ["//visibility:public"])"""

    maybe(
        http_archive,
        name = "openssl",
        build_file_content = all_content,
        strip_prefix = "openssl-1.1.1i",
        urls = ["https://www.openssl.org/source/openssl-1.1.1i.tar.gz"],
    )

    maybe(
        http_archive,
        name = "libevent",
        build_file_content = all_content,
        strip_prefix = "libevent-2.1.12-stable",
        urls = [
            "https://github.com/libevent/libevent/releases/download/release-2.1.12-stable/libevent-2.1.12-stable.tar.gz",
        ],
    )
