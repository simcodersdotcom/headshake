#ifndef MHELPERS_H
#define MHELPERS_H

#define FRAC_BIT_RES  12

/*
File: helpers.h

This file contains some useful mathematic functions.

*/

#include <vector>

float continue_log(float x);

/*
Get the average of the vector.
The first element is the more weighted.
*/
float average(const std::vector<float> &v);

/*
Get a weighted average of the vector.
The first element is the more weighted.
*/
float weighted_average(const std::vector<float> &v);

/*
Get the settings file path
*/
bool get_settings_filepath(char *path);

inline float quantize(float val)
{
  static const float factor = (float) (1<<FRAC_BIT_RES);
  return (float) ((int) (val * factor)) / factor;
}

#endif // HELPERS_H
