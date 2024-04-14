#ifndef __INTERPOLATION_H__
#define __INTERPOLATION_H__

void Lerp(int &value, float t, int destination) {
	value += (t) * (destination - value);
}

#endif // __INTERPOLATION_H__