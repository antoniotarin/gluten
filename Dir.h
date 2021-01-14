#ifndef DIR_H
#define DIR_H
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Dir
{
    public:
    // Coordenadas
    string name;
    float x, y, z;
    float level;
    vector<Dir*> children;
};

#endif // DIR_H
