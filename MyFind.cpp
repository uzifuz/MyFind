#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include <vector>
#include <filesystem>
#include <unistd.h>
#include <limits.h>
#include <sys/wait.h>

using namespace std;
using namespace filesystem;

void searchFile(path const &searchPath, string const &filename, bool insensitive, bool recursive, int *pipe)
{
    auto iter = directory_iterator{searchPath};
    auto r_iter = recursive_directory_iterator{searchPath};
    // for (auto file : iter)
    directory_entry file;
    /* counts how many files with the name have been found, lines 23, 35*/
    int filecounter = 0;
    string out = "";
    while (
        recursive
            ? ++r_iter != end(r_iter)
            : ++iter != end(iter))
    {
        file = recursive ? *r_iter : *iter;
        string otherName = file.path().filename();
        if (filename.compare(otherName) == 0 || (insensitive && strcasecmp(&filename[0], &otherName[0]) == 0))
        {
            /* adds each found file to the string to be written into the pipe, lines 36*/
            filecounter++;
            out += to_string(getpid()) + ": " + filename + ": " + (string)absolute(file) + "\n";
        }
    }
    /* if no files have been found instead output an error message, lines 34-43*/
    if(!filecounter){
        out += to_string(getpid()) + ": " + filename + ": file not found at searchPath\n";
        // std::cout << filename << ": file not found" << endl;
    }
    auto writing = fdopen(*pipe, "w");
    fputs(&out[0], writing);
    fclose(writing);
}

void inputErrorMessage() {
    cerror << "Input invald, make sure the command follows the following structure:" << endl;
    cerror << "myFind [i] [R] searchfile filename [filename]" << endl;
    cerror << "where i and R are optional and multiple filenames can be searched for" << endl;
    cerror << "Example: myFind i R testdirectory testfile1 testfile2 testfile3" << endl;
    return;
}

int main(int argc, char *argv[])
{
    bool recursive = 0;
    bool insensitive = 0;
    int opt;

    while ((opt = getopt(argc, argv, "Ri")) != -1)
    {
        switch (opt)
        {
        case 'i':
            insensitive = 1;
            break;
        case 'R':
            recursive = 1;
            break;
        case '?':
            assert(1);
            break;
        default:
            inputErrorMessage();
            assert(0);
            break;
        }
    }
    int options = recursive + insensitive;
    if (options + 2 >= argc)
    {
        inputErrorMessage();
        return EXIT_FAILURE;
    }
    path searchpath = "";
    vector<string> filenames;
    for (int iter = options + 1; iter < argc; iter++)
    {
        /* shortened the if statement */
        (iter == options +1) ? searchpath = argv[iter] : filenames.push_back(argv[iter]);
        /*if (iter == options + 1)
        {
            searchpath = argv[iter];
            // cout << "searchpath: " << argv[iter] << endl;
        }
        else
        {
            filenames.push_back(argv[iter]);
            // cout << "filename: " << argv[iter] << endl;
        }*/
    }

    int pid;

    int fd[2];
    FILE *reading;
    char buffer[PIPE_BUF];
    if (pipe(fd) < 0)
    {
        cerr << "error creating pipe" << endl;
        exit(EXIT_FAILURE);
    }

    for (auto filename : filenames)
    {
        pid = fork();
        switch (pid)
        {
        case -1:
            cerr << "fork error" << endl;
            break;
        case 0:
            // child
            close(fd[0]); // close reading end
            searchFile(searchpath, filename, insensitive, recursive, &fd[1]);
            exit(EXIT_SUCCESS);
        default:
            // parent
            // std::cout << "fork: " << pid << " " << filename << endl;
            break;
        }
    }

    close(fd[1]);
    reading = fdopen(fd[0], "r");

    // std::cout << getpid() << ") children: " << childPids.size() << endl;
    while (wait(NULL) != -1)
        ; // wait for children to terminate

    while (fgets(buffer, PIPE_BUF, reading) != NULL)
    {
        std::cout << buffer;
    }
    fclose(reading);

    return EXIT_SUCCESS;
}
