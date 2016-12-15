//gyroscope.h
#ifndef GYROSCOPE_H
#define GYROSCOPE_H

#include "reference.h"

class Gyroscope : public Reference {
    OBJ_TYPE(Gyroscope,Reference);

	int fd;
	char write_buffer[200];
	char read_buffer[200];

	int X;
	int Z;
	int Y;
protected:
	static void _bind_methods();
public:
	void gyroscope_init();
	int gyroscope_get_X();
	int gyroscope_get_z();
	int gyroscope_get_y();
	void gyroscope_reset();
	Gyroscope();
};

#endif