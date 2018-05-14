//
// Created by exomat on 20.03.18.
//

#ifndef PROJEKTSO_FUNCTION_H
#define PROJEKTSO_FUNCTION_H

void  compare_files(char *source, char *destination);
char* add_path(char *input, char *what_to_add);
void help();
void is_good_path(int argc,char** argv);
void deleting_files(char *source, char *destination);
int read_args(int argc, char**argv, char *source, char *destination);
int copy_nmap(char *source, char *destination, struct stat *file);
int copy_read_write(char *source, char *destination, mode_t mask);
int is_regular_file(const char *path);
void  start_copy(char *source_file_path, char *destination_file_path, struct stat *source_file);
void check_params(int argc, char *const *argv, int time, int size, int if_rec);
void if_file_handler_for_deleting(char *source_file_path, char *destination_file_path, struct stat *source_file);
void if_directory_handler_for_deleting(char *source_file_path, char *destination_file_path, struct stat *source_file, struct stat *desination_file);
void if_file_handler_for_fun_comp(char *source_file_path, char *destination_file_path, struct stat *source_file, struct stat *desination_file);
void if_is_directory_handler_for_fun_comp(char *source_file_path,  char *destination_file_path, struct stat *source_file, struct stat *desination_file);

#endif //PROJEKTSO_FUNCTION_H