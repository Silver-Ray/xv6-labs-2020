#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define RD 0
#define WR 1
#define perror(s) fprintf(2, "%s\n", s)
int main()
{
    int p2c[2]; // parent to child
    int c2p[2]; // child to parent
    if (pipe(p2c) == -1 || pipe(c2p) == -1)
    {
        perror("pipe failed");
        exit(-1);
    }
    if (fork() == 0) // child
    {
        close(p2c[WR]);
        close(c2p[RD]);
        if (read(p2c[RD], 0, 1) != 1)
        {
            perror("read failed");
            exit(-1);
        }
        else
        {
            printf("%d: received ping\n", getpid());
            close(p2c[RD]);
            write(c2p[WR], "p", 1);
            close(c2p[WR]);
            exit(0);
        }
    }
    else // parent
    {
        close(p2c[RD]);
        close(c2p[WR]);
        write(p2c[WR], "p", 1);
        close(p2c[WR]);
        if (read(c2p[RD], 0, 1) != 1)
        {
            perror("read failed");
            exit(-1);
        }
        else
        {
            printf("%d: received pong\n", getpid());
            close(c2p[RD]);
            exit(0);
        }
    }
}
