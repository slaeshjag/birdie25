#include "math.h"



double math_delta_to_angle(double dx, double dy) {
	double angle;

	if (dx == 0)
		angle = 0;
	else
		angle = ((double) dy) / ((double) dx);
	angle = atan(angle);
	if (dx < 0)
		angle = -1*angle + M_PI;
	else {
		angle = -1*angle;
		if (angle < 0)
			angle = 2. * M_PI + angle;
	}

	return angle;
}
