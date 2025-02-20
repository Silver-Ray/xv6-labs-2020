#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void find(const char *path, const char *name)
{
    int fd;
    char buf[512], *p;
    struct dirent de;
    struct stat st;
    if ((fd = open(path, 0)) < 0)
    {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0)
    {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }
    // 参数错误，find的第一个参数必须是目录
    if (st.type != T_DIR)
    {
        fprintf(2, "find: the %s is not a directory", name);
        close(fd);
        return;
    }

    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf))
    {
        fprintf(2, "find: path too long\n");
        exit(1);
    }

    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    while (read(fd, &de, sizeof(de)) == sizeof(de))
    {
        if (de.inum == 0)
            continue;
        memmove(p, de.name, DIRSIZ); // 添加路径名称
        p[DIRSIZ] = 0;               // 结束标志
        if (stat(buf, &st) < 0)
        {
            fprintf(2, "find: cannot stat %s\n", buf);
            continue;
        }

        // 不要在"."和".."目录中递归

        // de.name 当前读取到条目的名称

        if (st.type == T_DIR && strcmp(de.name, ".") != 0 && strcmp(de.name, "..") != 0)
        {
            find(buf, name);
        }
        else if (strcmp(de.name, name) == 0)
        {
            fprintf(2, "%s\n", buf);
        }
    }
    close(fd);
    // 不能调用exit(0)，否则扫完一个目录后会终止
    // exit(0);
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(2, "Usage: find <path> <file1> <file2>(optional) ...\n");
        exit(1);
    }
    else
    {
        for (int i = 2; i < argc; i++)
        {
            find(argv[1], argv[i]);
        }
    }
    exit(1);
}
