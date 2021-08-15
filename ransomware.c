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

int encrypt(
    void* buffer,
    int buffer_len, /* Because the plaintext could include null bytes*/
    char* IV,
    char* key,
    int key_len
){
  MCRYPT td = mcrypt_module_open("rijndael-128", NULL, "cbc", NULL);
  int blocksize = mcrypt_enc_get_block_size(td);
  if( buffer_len % blocksize != 0 ){return 1;}

  mcrypt_generic_init(td, key, key_len, IV);
  mcrypt_generic(td, buffer, buffer_len);
  mcrypt_generic_deinit (td);
  mcrypt_module_close(td);

  return 0;
}

int decrypt(
    void* buffer,
    int buffer_len,
    char* IV,
    char* key,
    int key_len
){
  MCRYPT td = mcrypt_module_open("rijndael-128", NULL, "cbc", NULL);
  int blocksize = mcrypt_enc_get_block_size(td);
  if( buffer_len % blocksize != 0 ){return 1;}

  mcrypt_generic_init(td, key, key_len, IV);
  mdecrypt_generic(td, buffer, buffer_len);
  mcrypt_generic_deinit (td);
  mcrypt_module_close(td);

  return 0;
}

char * find_directory_of_file(char * path){
    char * tmp = (char*)malloc(sizeof(char*)*1000);
    tmp = strrchr(path, '/');
    path[strlen(path) - strlen(tmp) + 1] = '\0';
    return path;
}

void encrypt_file(char * path){
    FILE *fp = fopen(path, "r");
    FILE *fp_temp = fopen("temp.tmp", "w");

    if (fp == NULL || fp_temp == NULL)
    {
        /* Unable to open file hence exit */
        printf("\nUnable to open file.\n");
        printf("Please check whether file exists and you have read/write privilege.\n");
        exit(EXIT_SUCCESS);
    }
    char* buffer = calloc(1, 1027);

    while ((fgets(buffer, 1026, fp)) != NULL)
    {
        //buffer[strlen(buffer) -1] = '\0';
        printf("%d\n", strlen(buffer));

        // Replace all occurrence of word from current line
        encrypt(buffer, 1026, IV, key, keysize);

        // After replacing write it to temp file.
        fputs(buffer, fp_temp);
    }
    free(buffer);
    fclose(fp);
    fclose(fp_temp);

    /* Delete original source file */
    remove(path);

    /* Rename temp file as original file */
    char * cmd = (char*) malloc(sizeof(char*)*1000);
    sprintf(cmd,"cp %s %s", "temp.tmp", path);
    system(cmd);
    free(cmd);
    remove("temp.tmp");
}

void decrypt_file(char * path){
    FILE *fp = fopen(path, "r");
    FILE *fp_temp = fopen("temp.tmp", "w");

    if (fp == NULL || fp_temp == NULL)
    {
        /* Unable to open file hence exit */
        printf("\nUnable to open file.\n");
        printf("Please check whether file exists and you have read/write privilege.\n");
        exit(EXIT_SUCCESS);
    }
    char* buffer = (char*)malloc(sizeof(char*)*1026);

    while ((fgets(buffer, 1027, fp)) != NULL)
    {
        printf("%d\n", strlen(buffer));
        // Replace all occurrence of word from current line
        decrypt(buffer, 1026, IV, key, keysize);
        printf("%s\n", buffer);
        // After replacing write it to temp file.
        fputs(buffer, fp_temp);
    }
    free(buffer);
    fclose(fp);
    fclose(fp_temp);

    /* Delete original source file */
    remove(path);

    /* Rename temp file as original file */
    char * cmd = (char*) malloc(sizeof(char*)*1000);
    sprintf(cmd,"cp %s %s", "temp.tmp", path);
    printf("%s\n", cmd);
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
        encrypt_file(list_all_files[i]);
        decrypt_file(list_all_files[i]);
    }
    //encrypt_file("/home/doanhnn/linux/test/word/document2.xml");
    //MCRYPT td, td2;
    //decrypt(buffer1, 16, IV, key, keysize);
    //printf("decrypt: %s\n", buffer1);
    //char* plaintext = "1234567890123456";
    //char* buffer;
    //int buffer_len = 20;

    //buffer = calloc(1, buffer_len);
    //strncpy(buffer, plaintext, buffer_len);
    //printf("%d\n", strlen(plaintext));
    //printf("%d\n", strlen(buffer));
    //printf("==C==\n");
    //printf("plain:   %s\n", buffer);
    //encrypt(buffer, buffer_len, IV, key, keysize); 
    //printf("cipher:  %s\n", buffer);
    //printf("%d\n", strlen(buffer));

    //decrypt(buffer, buffer_len, IV, key, keysize);
    //printf("decrypt: %s\n", buffer);
    return(0);
}
