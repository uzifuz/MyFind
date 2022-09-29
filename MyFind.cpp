#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    bool recursive = 0;
    bool sensitive = 0;
    int opt;

    while ((opt = getopt(argc, argv, "Ri")) != -1)
    {
        switch (opt)
        {
        case 'i':
            sensitive = 1;
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
    int options = recursive + sensitive;
    if (options + 2 >= argc)
    {
        cout << "too few arguments" << endl;
        return EXIT_FAILURE;
    }
    for (int iter = options + 1; iter < argc; iter++)
    {
        if (iter == options + 1)
        {
            cout << "searchpath: " << argv[iter] << endl;
        }
        else
        {
            cout << "filename: " << argv[iter] << endl;
        }
    }

    return EXIT_SUCCESS;
}
