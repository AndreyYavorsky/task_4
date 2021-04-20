#include <cstdio>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>
#include <unistd.h>

int is_file(const char *name) {
    struct stat stbuf;
    stat(name, &stbuf);
    return S_ISREG(stbuf.st_mode);
}

int walk_dir(const char *name) {
    int is_file_flag = is_file(name);

    if (is_file_flag != 0) {
        return EXIT_FAILURE;
    }

    DIR *dir = opendir(name);
    if (!dir) {
        return EXIT_FAILURE;
    }

    struct dirent *dent;
    pid_t new_proc_copy; 
    int count = 0;
    while ((dent = readdir(dir)) != NULL) {
        if ((strncmp(dent->d_name, ".", 1) == 0) ||
                        (strncmp(dent->d_name, "..", 2) == 0))
        {
            continue;
        }
        size_t fullpath_len = strlen(name) + strlen(dent->d_name) + 2;
        char fullpath[fullpath_len];
        sprintf(fullpath, "%s/%s", name, dent->d_name);
        int rv;
        count++;
		switch (new_proc_copy = fork())
		{
			case -1:
				perror("fork");
				exit(1);
			case 0:
				execl("./my_copy", "", (char *)fullpath, NULL);
				exit(rv);
		}
    }
    int rv;
    for(int i = 0 ; i < count; i++){
        wait(&rv);
    }
    closedir(dir);
    return EXIT_SUCCESS;
}


int main(int argc, char const *argv[]) {

	const char* dir = argv[1];

    if (walk_dir(dir) != EXIT_SUCCESS) {
        fprintf(stderr, "walk_dir\n");
        abort();
    }
    return 0;
}
