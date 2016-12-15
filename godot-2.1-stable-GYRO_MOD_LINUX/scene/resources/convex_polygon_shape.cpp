/*************************************************************************/
/*  convex_polygon_shape.cpp                                             */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                    http://www.godotengine.org                         */
/*************************************************************************/
/* Copyright (c) 2007-2016 Juan Linietsky, Ariel Manzur.                 */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/
#include "convex_polygon_shape.h"
#include "servers/physics_server.h"
#include "quick_hull.h"

Vector<Vector3> ConvexPolygonShape::_gen_debug_mesh_lines() {

	DVector<Vector3> points = get_points();

	if (points.size()>3) {

		QuickHull qh;
		Vector<Vector3> varr = Variant(points);
		Geometry::MeshData md;
		Error err = qh.build(varr,md);
		if (err==OK) {
			Vector<Vector3> lines;
			lines.resize(md.edges.size()*2);
			for(int i=0;i<md.edges.size();i++) {
				lines[i*2+0]=md.vertices[md.edges[i].a];
				lines[i*2+1]=md.vertices[md.edges[i].b];
			}
			return lines;


		}

	}

	return Vector<Vector3>();
}


void ConvexPolygonShape::_update_shape() {

	PhysicsServer::get_singleton()->shape_set_data(get_shape(),points);
	emit_changed();
}

void ConvexPolygonShape::set_points(const DVector<Vector3>& p_points) {

	points=p_points;
	_update_shape();
	notify_change_to_owners();
}

DVector<Vector3> ConvexPolygonShape::get_points() const {

	return points;
}


void ConvexPolygonShape::_bind_methods() {

	ObjectTypeDB::bind_method(_MD("set_points","points"),&ConvexPolygonShape::set_points);
	ObjectTypeDB::bind_method(_MD("get_points"),&ConvexPolygonShape::get_points);

	ADD_PROPERTY( PropertyInfo(Variant::ARRAY,"points"), _SCS("set_points"), _SCS("get_points") );

}

ConvexPolygonShape::ConvexPolygonShape() : Shape( PhysicsServer::get_singleton()->shape_create(PhysicsServer::SHAPE_CONVEX_POLYGON)) {

	//set_points(Vector3(1,1,1));
}