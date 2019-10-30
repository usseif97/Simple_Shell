#include <iostream>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <sys/wait.h>
#include <unistd.h>
#include <fstream>
// Clearing the shell using escape sequences
#define clear() printf("\033[H\033[J")

using namespace std;

char* username;




void init_shell() {
	clear();
	printf("\n\n\n\n******************"
		"************************");
	printf("\n\n\n\t****MY SHELL****");
	printf("\n\n\n\n*******************"
		"***********************");
    string line;
	username = getenv("USER");

	printf("\n\n\nUSER is: @%s", username);
	printf("\n");
    printf("press Enter To contine");
    getline(cin, line);
	clear();
}

void logProcessTermination() {
    ofstream file("Process termination.log", ios_base::app);
    file << "Child process was termiiiiiiiinaaaateeeeeed.\n";
}

void toCharArray(string& line, char* charArr) {
    for (unsigned i = 0; i < line.size(); i++) {
        charArr[i] = line.at(i);
    }
    charArr[line.size()] = '\0';
}

vector<char*> parseArgs(char* line) {
    vector<char*> argsVector;

    char* token = strtok(line, " ");
    while (token != nullptr) {
        argsVector.push_back(token);
        token = strtok(nullptr, " ");
    }
    return argsVector;
}

void signalHandler(int signum) {
    //check if the process is runing in the background
    if (signum==SIGCHLD) {
         while (waitpid(-1, NULL, WNOHANG) > 0 ) {
            //Process is terminated
            logProcessTermination();
        }
    }
}

void executeCommand(char* line) {
    vector<char*> argsVector = parseArgs(line);

    if (argsVector.empty()) {
        cout << "ERROR: There's No Command" << endl;
    } else {
        int background = 0;
        if (strcmp(argsVector.at(argsVector.size() - 1), "&") == 0){
            background = 1;
            argsVector.at(argsVector.size() - 1) = nullptr;

        } else {
            argsVector.push_back(nullptr);
        }

        char* args[argsVector.size()];
        for (int i = 0; i < argsVector.size(); i++){
            args[i] = argsVector[i];
        }

        int pid = fork();

        if (pid == 0){
            /* Never returns if the call is successful */
            execvp(args[0], args);
            cout << "ERROR: Failed To Execute Command" << endl;
            exit(-1);

        } else if (background == 0){
            wait(nullptr);
            //waitpid(pid, &stat_loc, WUNTRACED); // Wait for a specific child process to end
        } else {
            signal(SIGCHLD,signalHandler); // active the signal of the process which is running in the background
            cout << "Running in background" << endl;
        }

    }
}



int main() {

    init_shell();
    bool exit = false;
    string line;

    while (! exit){
        printf("%s@Ubuntu:$ ", username);
        getline(cin, line);

        if (line == "exit") {
            exit = true;
        } else {
            char lineArray[line.size() + 1];
            toCharArray(line, lineArray);
            executeCommand(lineArray);
        }

    }



    return 0;
}
