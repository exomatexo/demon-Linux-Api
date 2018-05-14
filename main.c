#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "function.h"
#include <signal.h>
#include <syslog.h>

int recursion = 0, sleep_time= 0 , size_file=0 ;
char  is_signal = 0;

void handler(){
    syslog(LOG_INFO,"Signal recived. Daemon wake up.");
    is_signal=1;
}
void signal_change_fun() {
    struct sigaction aciton_1;
    struct sigaction action2;
    aciton_1.sa_handler=handler;
    sigfillset(&aciton_1.sa_mask);
    action2.sa_flags=0;
    if (sigaction(SIGINT,&aciton_1,&action2)<0) {
        syslog(LOG_ERR,"Error: Cannot handle signal SIGUSR1.");
        closelog();
        exit(EXIT_FAILURE);
    }
}

void deafult_value_set() {
    /*
     * Deafult sleeping time - 5 min.
     */
    if (sleep_time==0){
        sleep_time=300;
    }
    /*
     * Default size file - 25B.
     */
    if(size_file==0){
        size_file=25;
    }
}

pid_t start_new_procces() {
    pid_t pid, sid;
    /* Fork off the parent process */
    pid = fork();
    if (pid < 0) {
        closelog();
        exit(EXIT_FAILURE);
    }
    /* If we got a good PID, then
       we can exit the parent process. */
    if (pid > 0) {
        closelog();
        exit(EXIT_SUCCESS);
    }

    /* Change the file mode mask */
    umask(0);

    /* Open any logs here */

    /* Create a new SID for the child process */
    sid = setsid();
    if (sid < 0) {
        /* Log any failures here */
        syslog (LOG_ERR, "Cannot create new SID.");
        syslog (LOG_NOTICE, "Daemon Stoped.");
        closelog();
        exit(EXIT_FAILURE);
    }
    return sid;
}

int main(int argc, char **argv) {

    /*
     * change default SIGUSR1 ability!! XD
    */
    signal_change_fun();
    char  source[1000], destination[1000];


    openlog("SO-Projekt-Daemon:",LOG_PID | LOG_CONS, LOG_USER);
    syslog(LOG_INFO,"Opening Log File.");
    /*
     * check if its a good path
     */


    read_args(argc,argv,source,destination);
    signal(SIGUSR1, handler);
    deafult_value_set();

    pid_t sid = start_new_procces();

    // print the int number of process so we could easy send signal with kill  ;)
    printf("\n[!] %d\n",sid);

    syslog (LOG_NOTICE, "Starting Daemon with Parametrs: sleep_time=%d size_file=%d recursion=%d", sleep_time, size_file, recursion);

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    syslog (LOG_NOTICE,"Closing input, output, error output stream.");

    while(1){

        syslog (LOG_NOTICE, "Started comparing files. compare_files().");
        compare_files(source,destination);
        syslog (LOG_NOTICE, "Finished comparing files. compare_files().");

        syslog (LOG_NOTICE, "Started deliting files. deleting_files().");
        deleting_files(source,destination);
        syslog (LOG_NOTICE, "Finished deleting files. deleting_files().");

        syslog (LOG_NOTICE, "Sleeping. sleep(%d).",sleep_time);
        sleep(sleep_time);

        if(!is_signal) {
            syslog(LOG_NOTICE, "Sleeping time has passed. Daemon wake up");
            is_signal=0;
        }
    }
    closelog();
        return 0;
}