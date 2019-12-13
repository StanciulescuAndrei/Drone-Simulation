#include "PIDr.h"

PIDr::PIDr(int n, Movement* M) {
	ref = new float[n];
	err = new float[n];
	lasterr = new float[n];
	integr = new float[n];
	deriv = new float[n];
	mvmt = M;
	Ki = 0;
	Kd = 0.05;
	Kp = 1;
	dim = n;
	T = 0;

	for (int i = 0; i < n; i++) {
		ref[i] = 0;
		err[i] = 0;
		lasterr[i] = 0;
		integr[i] = 0;
		deriv[i] = 0;
	}
}

void PIDr::calcAngles(double vx, double vy, double vz) {
	T = sqrt(vx * vx + vy * vy + vz * vz);
	if (T == 0)
	{
		ref[0] = 0;
		ref[1] = 0;
		return;
	}

	if (vy < 0)
		T = -T;

	if (vx >= 0 && vy >= 0) {
		std::cout << "ref1 " << ((T - vx) / T) - (int)((((T - vx) / T) / 2) * 2) << std::endl;
		std::cout << "ref2 " << (vy / (T - vx)) - (int)(((vy / (T - vx)) / 2) * 2) << std::endl;

		ref[0] = acos( ((T - vx) / T) - (int)((((T - vx) / T) / 2)*2));
		ref[1] = acos( (vy / (T - vx)) - (int)(((vy / (T - vx))/2)*2));
	}
	else if (vx >= 0 && vy < 0) {
		std::cout << "ref1 " << ((T - vx) / T) - (int)((((T - vx) / T) / 2) * 2) << std::endl;
		std::cout << "ref2 " << ((T - vx - vz) / (T - vx)) - (int)((((T - vx - vz) / (T - vx)) / 2) * 2) << std::endl;

		ref[0] = acos(((T - vx) / T) - (int)((((T - vx) / T) / 2) * 2));
		ref[1] = acos(((T - vx - vz) / (T - vx)) - (int)((((T - vx - vz) / (T - vx))/2)*2));
	}
	else if (vx < 0 && vy >= 0) {

		std::cout << "ref1 " << ((vx + T) / T) - (int)((((vx + T) / T) / 2) * 2) << std::endl; 
		std::cout << "ref2 " << (vy / (T - vx)) - (int)(((vy / (T - vx)) / 2) * 2) << std::endl;

		ref[0] = acos( ((vx + T) / T) - (int)((((vx + T) / T)/2)*2)) ;
		ref[1] = acos((vy / (T - vx)) - (int)(((vy / (T - vx)) / 2) * 2));
	}
	else if (vx < 0 && vy < 0) {
		std::cout << "ref1 " << ((vx + T) / T) - (int)((((vx + T) / T) / 2) * 2) << std::endl;
		std::cout << "ref2 " << ((T - vx - vz) / (T - vx)) - (int)((((T - vx - vz) / (T - vx)) / 2) * 2) << std::endl;

		ref[0] = acos(((vx + T) / T) - (int)((((vx + T) / T) / 2) * 2));
		ref[1] = acos(((T - vx - vz) / (T - vx)) - (int)((((T - vx - vz) / (T - vx)) / 2) * 2));
	}
}

void PIDr::update() {
	err[0] = ref[0] - mvmt->roll;
	err[1] = ref[1] - mvmt->pitch;

	for (int i = 0; i < dim; i++) {

		integr[i] += err[i] * mvmt->time_step;
		deriv[i] = err[i] - lasterr[i];

	}
	double c_roll = Kp * err[0] + Ki * integr[0] + Kd * deriv[0];
	double c_pitch = Kp * err[1] + Ki * integr[1] + Kd * deriv[1];

	mvmt->forte[0].setMag(T / 4 - c_pitch / 2 + c_roll / 2 + mass * 2.5);
	mvmt->forte[1].setMag(T / 4 - c_pitch / 2 - c_roll / 2 + mass * 2.5);
	mvmt->forte[2].setMag(T / 4 + c_pitch / 2 + c_roll / 2 + mass * 2.5);
	mvmt->forte[3].setMag(T / 4 + c_pitch / 2 - c_roll / 2 + mass * 2.5);

	for (int i = 0; i < dim; i++) {
		lasterr[i] = err[i];
	}
}
