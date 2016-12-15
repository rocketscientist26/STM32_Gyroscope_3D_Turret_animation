#include "register_types.h"
#include "object_type_db.h"
#include "gyroscope.h"

void register_gyroscope_types() {
	ObjectTypeDB::register_type<Gyroscope>();
}
void unregister_gyroscope_types() {
}
