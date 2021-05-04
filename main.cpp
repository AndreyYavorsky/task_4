#include <cstdio>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pthread.h>
#include <semaphore.h>
#include <fstream>
#include <string>
#include <vector>


struct in_arg {
    const char* name_file;
    int number_pthread;
};

static in_arg global_buff;
static pthread_mutex_t mutex_deposit;
static pthread_mutex_t mutex_fetch;



int is_file(const char *name) {
    struct stat stbuf;
    stat(name, &stbuf);
    return S_ISREG(stbuf.st_mode);
}

int my_copy(const char* name)
{
    const int buf_size = 10240; // Буфер 10 КБ
    std::string src_file = name;
    int is_file_flag = is_file(src_file.c_str());

    if (is_file_flag != 1) {
        return 0;
    }
    
    std::string out_file = src_file;
    size_t position_point = 0; 
    position_point = out_file.find("." , 0);
    if (position_point == std::string::npos) {
        out_file = out_file + "-copy";
    }
    else {
        out_file.insert(position_point, "-copy");
    }
    std::ifstream ifs(src_file.c_str(), std::ios::binary);
 
    ifs.seekg(0, std::ios::end);
    std::ios::pos_type src_size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
 
    size_t int_round = src_size / buf_size;
    size_t rest_file = src_size % buf_size;
    std::ofstream ofs(out_file.c_str(), std::ios::binary);
    char buf[buf_size];
    for(size_t i = 0; i < int_round; ++i){
        ifs.read(buf, buf_size);
        ofs.write(buf, buf_size);
    }
    if(rest_file != 0) {
        ifs.read(buf, rest_file);
        ofs.write(buf, rest_file);
    }
    ifs.close();
    ofs.close();
    return 0;
}

void* main_pthread(void* arg) {
    in_arg tmp_arg = *((in_arg*)arg);
    my_copy(tmp_arg.name_file);

    pthread_mutex_lock(&mutex_fetch);
    global_buff.name_file = tmp_arg.name_file;
    global_buff.number_pthread = tmp_arg.number_pthread;
    pthread_mutex_unlock(&mutex_deposit);

    pthread_exit(NULL);
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
    int count = 0;
    std::vector<std::string>  arr_fullpath;
    while ((dent = readdir(dir)) != NULL) {
        if ((strncmp(dent->d_name, ".", 1) == 0) ||
                        (strncmp(dent->d_name, "..", 2) == 0))
        {
            continue;
        }
        size_t fullpath_len = strlen(name) + strlen(dent->d_name) + 2;
        char fullpath[fullpath_len];
        sprintf(fullpath, "%s/%s", name, dent->d_name);
        arr_fullpath.push_back(fullpath);
        count++;
    }

    pthread_t pthread_arr[count];
    in_arg tmp_in_arg[count];
    for(int i = 0 ; i < count; i++) {
        (tmp_in_arg[i]).name_file = (arr_fullpath[i]).c_str();
        (tmp_in_arg[i]).number_pthread = i;
    }

    if (pthread_mutex_init(&mutex_deposit, NULL) ||
        pthread_mutex_init(&mutex_fetch, NULL)) {
        fprintf(stderr, "Failed to init mutex\n");
        return EXIT_FAILURE;
    }
    pthread_mutex_lock(&mutex_deposit);

    for(int i = 0 ; i < count; i++) {
        pthread_create( &(pthread_arr[i]), NULL, main_pthread, (void*)&(tmp_in_arg[i]) );
    }

    for (int i = 0; i < count; i++) {        
        pthread_mutex_lock(&mutex_deposit);
        printf("Поток номер %d  скопировал файл %s\n", global_buff.number_pthread + 1, global_buff.name_file);
        printf("------------------------------------------------\n");
        pthread_mutex_unlock(&mutex_fetch);
    }

    for(int i = 0 ; i < count; i++) {
        pthread_join(pthread_arr[i], NULL);
    }

    closedir(dir);
    printf("%s\n", "Все потоки завершили копирование" );
    arr_fullpath.clear();
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
