#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>

#include "init.h"
#include "server.h"
#include "util.h"

static void _print_help(void);
static void _print_hint(void);
static void _print_hint_bad(void);
static void _print_version(void);

int init(int argc, char *argv[]) {
    int opt;
    char *site_directory = NULL;

    struct option long_opts[] = {
        {"help", no_argument, NULL, 'h'},
        {"version", no_argument, NULL, 'V'},
        {NULL, 0, NULL, 0}
    };

    while ((opt = getopt_long(argc, argv, "hV", long_opts, NULL)) != -1) {
        switch (opt) {
            case 'h':
                _print_help();
                return (1);
            case 'V':
                _print_version();
                return (1);
            default:
                _print_hint_bad();
                return (1);
        }
    }

    if (optind < argc) {
        site_directory = argv[optind];
    } else {
        _print_hint();
        return (1);
    }

    if (chdir(site_directory) == -1) {
        perror("Error changing directory");
        return (1);
    }

    if (access("server.ini", F_OK) == -1) {
        fprintf(stderr, "Error: 'server.ini' not found in the specified directory\n");
        return (1);
    }

    return (0);
}

static void
_print_help(void)
{
    printf("%s: "PROGRAM" [%s] <%s>\n", _("Usage"), _("options"), _("site_directory"));
    printf("\n%s:\n", _("Options"));
    printf("  -h, --help      %s\n", _("displat this help"));
    printf("  -V, --version   %s\n", _("display version"));
    printf("\n%s "PROGRAM"(1).\n", _("For more details see"));
}

static void
_print_hint(void)
{
    printf(PROGRAM": %s\n", _("not enough argument"));
    printf("%s \'"PROGRAM" --help\' %s\n", _("Try"), _("for more information"));
}

static void
_print_hint_bad(void)
{
    printf(PROGRAM": %s\n", _("bad usage"));
    printf("%s \'"PROGRAM" --help\' %s\n", _("Try"), _("for more information"));
}

static void
_print_version()
{
    printf(PROGRAM" version "VERSION"\n");
}