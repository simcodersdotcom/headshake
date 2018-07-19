/*
File: helpers.cpp

This file contains some useful mathematic functions.

*/

#include <cmath>
#include <vector>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <string>

#include <CHeaders/XPLM/XPLMPlugin.h>
#include <CHeaders/XPLM/XPLMUtilities.h>


/*
Define a continue log function.
y = -log(|x - 1|) when x < 0
y = log(|x + 1|) when x >= 0
*/
float continue_log(float x)
{
    if (x < 0)
        return -log(std::fabs(x - 1));
    else
        return log(x + 1);
}

/*
Get the average of the vector.
*/
float average(const std::vector<float> &v)
{
    float average = 0;
    unsigned int i, arrSize = v.size();

    if (arrSize == 0)
        return 0;

    for (i = 0; i < arrSize; i++) {
        average += v.at(i);
    }
    return average / arrSize;
}

/*
Get a weighted average of the vector.
The first element is the more weighted.
*/
float weighted_average(const std::vector<float> &v)
{
    float average = 0;
    float count = 0;
    float farrSize = (float)v.size();
    unsigned int iarrSize = v.size();
    unsigned int i;

    if (iarrSize == 0)
        return 0;

    for (i = 0; i < iarrSize; i++) {
        average += v.at(i) * (farrSize - i);
        count += (farrSize - i);
    }
    return average / count;
}

/*
Get the settings file path
*/
bool get_settings_filepath(char *path)
{
    XPLMGetPrefsPath(path);
    // for an SDK bug which is there since 6.70, the returned path is not
    // the folder's path but the path of the file "Set X-Plane.prf". So let's
    // strip out the filename to get the folder name.
    XPLMExtractFileAndPath(path);
    // Add a directory separator to the end
    // (the function above modifies path with no trailing directory separator)
    strcat(path, XPLMGetDirectorySeparator());
    // Add the correct filename
    strcat(path, "HeadShake.prf");
    return true;
}
