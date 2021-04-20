#include <fstream>
#include <string>
#include <sys/stat.h>

int is_file(const char *name) {
    struct stat stbuf;
    stat(name, &stbuf);
    return S_ISREG(stbuf.st_mode);
}

int main(int argc, char const *argv[])
{
    const int buf_size = 10240; // Буфер 10 КБ
 
    // Имя исходного файла
    std::string src_file = argv[1];

    // Проверка на файл
    int is_file_flag = is_file(src_file.c_str());

    if (is_file_flag != 1) {
        return 0;
    }
    
    // Имя файла копии
    std::string out_file = src_file;
    size_t position_point = 0; 
    position_point = out_file.find("." , 0);
    if (position_point == std::string::npos)
    {
        out_file = out_file + "-copy";
    }
    else 
    {
        out_file.insert(position_point, "-copy");
    }

    // Создание потока для работы с исходным файлом
    std::ifstream ifs(src_file.c_str(), std::ios::binary);
 
    // Подсчет размер исходного файла
    ifs.seekg(0, std::ios::end);
    std::ios::pos_type src_size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
 
    // Количество целых проходов (для частей файлы, которые полностью умещаются в буфер)
    size_t a_mount = src_size / buf_size;
    // Остаток (остаток файла)
    size_t b_mount = src_size % buf_size;
    
    // Создание потока для файла-копии
    std::ofstream ofs(out_file.c_str(), std::ios::binary);

    char buf[buf_size];
 
    // Цикл по числу полных проходов
    for(size_t i = 0; i < a_mount; ++i)
    {
        ifs.read(buf, buf_size);
        ofs.write(buf, buf_size);
    }
    
    // Если есть остаток
    if(b_mount != 0)
    {
        ifs.read(buf, b_mount);
        ofs.write(buf, b_mount);
    }
 
    ifs.close();
    ofs.close();
 
    return 0;
}