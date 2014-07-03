#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>

void usage() {
    fprintf(stderr, "Usage: timingattack [options...] <invocation>\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  --l, --length      \t specify password length (20 is default)\n");
    fprintf(stderr, "  --i, --iterations  \t specify iterations per character (100 is default)\n");
    fprintf(stderr, "  --c, --charset     \t specify possible password characters (alphanum is default)\n");
    fprintf(stderr, "  --p, --prefer-slow \t choose the slowest iteration (fastest is default)\n");
    fprintf(stderr, "  --q, --quiet       \t only output final result\n");
    fprintf(stderr, "  --v, --verbose     \t print timing results after iterations\n");
    _exit(1);
}

int main(int argc, char **argv) {
    int quiet = 0;
    int verbose = 0;
    int prefer_slow = 0;
    static int verbose_flag;
    unsigned int iterations = 100;
    unsigned int length = 20;
    char alphanum[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ" \
                      "abcdefghijklmnopqrstuvwxyz" \
                      "0123456789";
    char *charset = alphanum;
    int c;
    while (1) {
        static struct option long_options[] = {
            { "length",      required_argument, 0, 'l' },
            { "iterations",  required_argument, 0, 'i' },
            { "charset",     required_argument, 0, 'c' },
            { "prefer-slow", no_argument,       0, 'p' },
            { "quiet",       no_argument,       0, 'q' },
            { "verbose",     no_argument,       0, 'v' },
            { 0,             0,                 0,  0  }
        };
        int option_index = 0;
        c = getopt_long(argc, argv, "i:", long_options, &option_index);
        if (c == -1)
            break;
        switch (c) {
        case 'i':
            iterations = atoi(optarg);
            break;
        case 'l':
            length = atoi(optarg);
            break;
        case 'c':
            charset = optarg;
            break;
        case 'p':
            prefer_slow = 1;
            break;
        case 'q':
            quiet = 1;
            break;
        case 'v':
            verbose++;
            break;
        default:
            usage();
        }
    }

    if (argc < 2 || argc - optind < 1)
        usage();

    struct timespec begin, end;
    unsigned long time, best_time;
    char password[length];
    pid_t pid;
    int i, ii, iii;

    for (i = 0; i < length; i++) {
        char best_candidate;
        for (ii = 0; ii < strlen(charset); ii++) {
            password[i] = charset[ii];
            password[i+1] = '\0';
            char *command[] = { argv[optind], password };
            clock_gettime(CLOCK_REALTIME, &begin);
            for (iii = 0; iii < iterations; iii++) {
                int status = 0;
                if (!(pid = fork())) {
                    execvp(command[0], command);
                } else if (pid < 0) {
                    return 1;
                } else {
                    wait(&status);
                }
            }
            clock_gettime(CLOCK_REALTIME, &end);
            time = (unsigned long)(end.tv_nsec - begin.tv_nsec)/iterations;
            if (!quiet) {
                if (verbose)
                    printf("%c: %lu\n", charset[ii], time);
                else
                    printf("%s\n", password);
            }
            if (!ii || (time < best_time && !prefer_slow)
                    || (time > best_time && prefer_slow)) {
                best_time = time;
                best_candidate = charset[ii];
            }
        }
        if (verbose)
            printf("Best run ('%c'): %lu\n", best_candidate, best_time);
        password[i] = best_candidate;
    }

    printf("\nPassword: %s\n", password);

    return 0;
}
