#define NL "\n"

#define A_RES "\x1b[0m"
#define A_STR "\x1b[1m"
#define A_RED "\x1b[31m"
#define A_GRN "\x1b[32m"
#define A_CYN "\x1b[36m"

#define ERR A_STR A_RED

#define P_STAT "[" A_CYN "STAT" A_RES "] "
#define P_GAME "[" A_GRN "GAME" A_RES "] "
#define STAT(msg)  { printf((P_STAT msg NL)); }
#define PEXIT(msg) { perror((ERR "[ERR] " msg)); exit(EXIT_FAILURE); }
