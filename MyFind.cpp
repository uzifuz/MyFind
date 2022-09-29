#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include <vector>
#include <filesystem>

using namespace std;
using namespace filesystem;

void searchFile(path const &searchPath, string const &filename, bool insensitive, bool recursive)
{
    auto iter = directory_iterator{searchPath};
    auto r_iter = recursive_directory_iterator{searchPath};
    // for (auto file : iter)
    directory_entry file;
    while(
        recursive 
        ? ++r_iter != end(r_iter)
        : ++iter != end(iter))
    {
        file = recursive ? *r_iter : *iter;
        string otherName = file.path().filename();
        if (filename.compare(otherName) == 0 || (insensitive && strcasecmp(&filename[0], &otherName[0]) == 0))
        {
            cout << filename << ": " << file.path() << endl;
        }
    }
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
            assert(1); /*invalid argument given*/
            break;
        default:
            assert(0);
            break;
        }
    }
    int options = recursive + insensitive;
    if (options + 2 >= argc)
    {
        cout << "too few arguments" << endl;
        return EXIT_FAILURE;
    }
    path searchpath = "";
    vector<string> filenames;
    for (int iter = options + 1; iter < argc; iter++)
    {
        if (iter == options + 1)
        {
            searchpath = argv[iter];
            // cout << "searchpath: " << argv[iter] << endl;
        }
        else
        {
            filenames.push_back(argv[iter]);
            // cout << "filename: " << argv[iter] << endl;
        }
    }

    for (auto filename : filenames)
    {
        searchFile(searchpath, filename, insensitive, recursive);
    }

    return EXIT_SUCCESS;
}
