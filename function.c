//
// Created by exomat on 20.03.18.
//
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include "function.h"
#include <fcntl.h>
#include <syslog.h>
#include <unistd.h>
#include <sys/mman.h>
#include <dirent.h>

char temp[1000];
int recursion, sleep_time, size_file;


void if_file_handler_for_deleting(char *source_file_path, char *destination_file_path, struct stat *source_file) {
    if(stat(source_file_path, source_file) == 0){
        
                }else{
                    if(unlink(destination_file_path)==0){
                        syslog (LOG_NOTICE, "deleting_files(): Unlink regular file succes: %s",destination_file_path);
                    } else{
                        syslog (LOG_ERR, "deleting_files(): Unlink regular file faild: %s",destination_file_path);
                    }
                }
    
}

void if_directory_handler_for_deleting(char *source_file_path, char *destination_file_path, struct stat *source_file, struct stat *desination_file) {
    if(S_ISDIR((*desination_file).st_mode) != 0 && recursion == 1) {
                if (stat(source_file_path, source_file) == 0) {
                    deleting_files(source_file_path,destination_file_path);
                    if (S_ISDIR((*source_file).st_mode) != 0) {
                        
                    } else {
                        syslog (LOG_ERR, "deleting_files(): %s is not a direcotry.",source_file_path);
                    }
                }
                else{
                    deleting_files(source_file_path,destination_file_path);
                    if(rmdir(destination_file_path)==0){
                        syslog (LOG_NOTICE, "deleting_files(): Delete a directory:%s Date: ",destination_file_path);
                    }else{
                        syslog (LOG_ERR, "deleting_files(): Failed to delete direcotry:%s",destination_file_path);
                    }
                }
            }
    
}
void if_file_handler_for_fun_comp(char *source_file_path, char *destination_file_path, struct stat *source_file, struct stat *desination_file) {
    if (stat(destination_file_path, desination_file) == 0) {
        if (((*source_file).st_mtime - (*desination_file).st_mtime) > 0) {
            start_copy(source_file_path, destination_file_path, source_file);
        }
    }else{
        start_copy(source_file_path, destination_file_path, source_file);
    }
}
 void if_is_directory_handler_for_fun_comp(char *source_file_path,  char *destination_file_path, struct stat *source_file, struct stat *desination_file) {
    if(stat(destination_file_path, desination_file) == 0){
                    if(S_ISDIR((*desination_file).st_mode) != 0){
                        compare_files(source_file_path,destination_file_path);
                    }else{
                        syslog (LOG_NOTICE, "compare_files(): %s is not a directory.", destination_file_path);
                    }
                }else{
                    if(mkdir(destination_file_path, (*source_file).st_mode) == 0){
                        compare_files(source_file_path,destination_file_path);
                    }
                    else{
                        syslog (LOG_ERR, "compare_files(): Cannot create a file: %s",destination_file_path);
                    }
                }
}

void  start_copy(char *source_file_path, char *destination_file_path, struct stat *source_file) {
    if (size_file > (*source_file).st_size) {
        if (copy_read_write(source_file_path, destination_file_path, (*source_file).st_mode) == 0) {
            syslog (LOG_NOTICE, "start_copy(): Succes to copy a file: %s", source_file_path);
        } else {
            syslog (LOG_ERR, "start_copy(): Failed to copy a file: %s", source_file_path);
        }
    }else{
        if(copy_nmap(source_file_path, destination_file_path, source_file) == 0){


            syslog (LOG_NOTICE, "start_copy(): Succes to copy a file by maping: %s", source_file_path);
        }else{
            syslog (LOG_ERR, "start_copy(): Failed to copy a file by maping: %s", source_file_path);
        }
    }
}

char* add_path(char *input, char *what_to_add){

    strcpy(temp,input);
    if(temp[strlen(temp)-1]!='/')
        strcat(temp,"/");
    strcat(temp,what_to_add);
    return temp;
}
void deleting_files(char *source, char *destination){
    char source_file_path[1000];
    char destination_file_path[1000];
    DIR *destination_dir;
    struct dirent *open_destination;
    struct stat source_file, desination_file;

    if ((destination_dir=opendir(destination)) != NULL){
        while ((open_destination=readdir(destination_dir))!=NULL){
            if ((strcmp(open_destination->d_name, ".") == 0) || (strcmp(open_destination->d_name, "..") == 0))
                continue;
            strcpy(source_file_path, add_path(source, open_destination->d_name));
            strcpy(destination_file_path, add_path(destination, open_destination->d_name));
            if(stat(destination_file_path, &desination_file)==-1){
                syslog (LOG_ERR, "deleting_files(): Error with destination_file_path: %s - stat().",destination_file_path);
                return;
            }
            /*
            * if its a file
            */
            if (is_regular_file(destination_file_path)){
                if_file_handler_for_deleting(source_file_path, destination_file_path, &source_file);
                continue;
            }else
                /*
                * if its a directory
                */
                if_directory_handler_for_deleting(source_file_path, destination_file_path, &source_file, &desination_file);
        }
    }else{
        syslog (LOG_ERR, "deleting_files(): Cannot open destinaton directory: %s",destination);
        return;
    }
}



void  compare_files(char *source, char *destination){
    char source_file_path[1000];
    char destination_file_path[1000];
    DIR *source_dir;
    struct dirent *open_source;
    struct stat source_file, desination_file;
    if ((source_dir=opendir(source)) != NULL){

        while ((open_source=readdir(source_dir))!=NULL) {

            if ((strcmp(open_source->d_name, ".") == 0) || (strcmp(open_source->d_name, "..") == 0))
                continue;

            strcpy(source_file_path, add_path(source, open_source->d_name));
            strcpy(destination_file_path, add_path(destination, open_source->d_name));
            if(stat(source_file_path, &source_file)==-1){

                syslog (LOG_NOTICE, "compare_files(): Error with source_file_path: %s in function stat().", source_file_path);
                return;
            }

            /*
             * if its a file
             */
            if (is_regular_file(source_file_path)) {
                if_file_handler_for_fun_comp(source_file_path, destination_file_path, &source_file, &desination_file);
            }
            else{
                if(recursion==1){
                    if_is_directory_handler_for_fun_comp(source_file_path, destination_file_path, &source_file, &desination_file);
                }
            }
        }
    }else{
        syslog (LOG_ERR, "compare_files(): Cannot open source directory: %s",source);
        return;
    }
    closedir(source_dir);
}


int is_regular_file(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}
void is_good_path(int argc,char** argv) {

    if (argc < 3) {
        syslog (LOG_ERR, "read_args(): Not enough arguments.");
        syslog (LOG_NOTICE, "Daemon Stoped.");
        help();
        closelog();
        exit(EXIT_FAILURE);
    }
    if (argc > 8) {
        syslog (LOG_ERR, "read_args(): To many arguments.");
        syslog (LOG_NOTICE, "Daemon Stoped.");
        help();
        closelog();
        exit(EXIT_FAILURE);
    }
    DIR *temp , *temp2;
    temp2=opendir(argv[2]);
    temp=opendir(argv[1]);
    if(temp == NULL || temp2 == NULL ){

        closedir(temp);
        closedir(temp2);
        syslog (LOG_ERR, "read_args(): Source path or Destinaton path is not a directory.");
        syslog (LOG_NOTICE, "Daemon Stoped.");
        closelog();
        exit(EXIT_FAILURE);
    }
}

void check_params(int argc, char *const *argv, int time, int size, int if_rec) {
    for (int i =3 ; i < argc; i++){
            if(strcmp(argv[i],"-T")==0 && time==0 && argv[i+1] !=NULL){
                time=1;
                sleep_time=atoi(argv[i+1]);
            }
            if(strcmp(argv[i],"-S")==0 && size==0 && argv[i+1] !=NULL){
                size=1;
                size_file=atoi(argv[i+1]);
            }
            if(strcmp(argv[i],"-R")==0 && if_rec==0){
                recursion=1;
                if_rec=1;
            }
        }
}
int read_args(int argc, char**argv, char *source, char *destination) {
    int time = 0, size = 0, if_rec=0;

    is_good_path(argc,argv);
    strcpy(source,argv[1]);
    strcpy(destination,argv[2]);
    check_params(argc, argv, time, size, if_rec);

    return 0;
}
void help(){
    printf("\n----------------------");
    printf("\n---------HELP---------");
    printf("\n----------------------");
    printf("\ninput paramets:");
    printf("\nprogram_name <source> <destination>");
    printf("\nOptional:");
    printf("\nTime in seconds: <-T time> ");
    printf("\nRecursion: <-R >");
    printf("\nSize to use nmap copy: <-S size>\n");
}

int copy_read_write(char *source, char *destination, mode_t mask){
    int file_in, file_out;
    int how_long;
    size_t buffer=10000;
    char bufor[buffer];
/*
 * open files
 */
    if( (file_in=open(source, O_RDONLY))<0)
    {
        syslog(LOG_ERR, "copy_read_write(): Cannot open file!! Path: %s\n",source);
        return -1;
    }

    if( (file_out=open(destination, O_CREAT | O_WRONLY | O_TRUNC, mask))<0)
    {
        syslog(LOG_ERR, "copy_read_write(): Cannot open file!! Path:  %s\n",destination);
        return -1;
    }


/*
 * copying files
 */

    while(how_long=read(file_in,bufor,buffer))
    {
        if(how_long==-1)
        {
            syslog(LOG_ERR, "copy_read_write(): Cannot read file: %s\n",source);
            close(file_in);
            close(file_out);
            return -1;
        }
        if(write(file_out,bufor,how_long)==-1)
        {
            syslog(LOG_ERR, "copy_read_write(): Cannot write file: %s\n",destination);
            close(file_in);
            close(file_out);
            return -1;
        }
    }
    close(file_in);
    close(file_out);
    return 0;
}
int copy_nmap(char *source, char *destination, struct stat *file)
{
    int file_in, file_out;
    char *temp;

/*
 * open files
 */
    if( (file_in=open(source, O_RDONLY))==-1)
    {
        syslog(LOG_ERR, "copy_nmap(): Cannot open file!! Path: %s\n",source);
        return -1;
    }
    if( (file_out=open(destination, O_CREAT | O_WRONLY | O_TRUNC, file->st_mode))<0)
    {
        syslog(LOG_ERR, "copy_nmap(): Cannot open file!! Path: %s\n",destination);
        return -1;
    }
/*
 * copying files
 */
    if((temp=mmap(0, (size_t) file->st_size, PROT_READ, MAP_SHARED, file_in, 0)) < 0 )
    {
        syslog(LOG_ERR, "copy_nmap(): Cannot read file: %s\n",source);
        close(file_in);
        close(file_out);
        return -1;
    }
    if(write(file_out, temp, (size_t) file->st_size) ==-1)
    {
        syslog(LOG_ERR, "copy_nmap(): Cannot write file: %s\n",destination);
        close(file_in);
        close(file_out);
        return -1;
    }
    munmap(temp,file->st_size);
    close(file_in);
    close(file_out);
    return 0;
}




