#include <dirent.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>

char** list_all_files;
int count_files = 0;

void list_files(char * path){
    DIR *d = opendir(path);
    struct dirent *dir;
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            //Condition to check regular file.
            if(dir->d_type==DT_REG){
                list_all_files[count_files] = (char*)malloc(1000);
                sprintf(list_all_files[count_files++],"%s/%s",path, dir->d_name);
            }
            else if (dir -> d_type == DT_DIR && strcmp(dir->d_name,".")!=0 && strcmp(dir->d_name,"..")!=0 ){
                char d_path[255];
                sprintf(d_path, "%s/%s", path, dir->d_name);
                list_files(d_path); 
            }
        }
        closedir(d);
    }
}

int main()
{
    list_all_files = malloc(1000*sizeof(char*));

    char path[1000]="/home/doanhnn/linux/test";
    list_files(path);
    for(int i =0; i< count_files; i++)
        printf("%s\n", list_all_files[i]);
    return(0);
}
