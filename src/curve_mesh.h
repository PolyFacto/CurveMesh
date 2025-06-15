#pragma once

#include <godot_cpp/classes/curve3d.hpp>
#include <godot_cpp/classes/mesh.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>

using namespace godot;

class CurveMesh : public RefCounted 
{
	GDCLASS(CurveMesh, RefCounted)

protected:
	static void _bind_methods();

public:
	CurveMesh();
	~CurveMesh() override = default;

	void init(const Ref<Mesh> &p_mesh, const bool p_cubic, const bool p_up_vector);
	Ref<Mesh> create_curve_mesh(const Vector3 p_start_pos, const Vector3 p_start_tangent, const float p_start_tilt, const Vector3 p_end_pos, const Vector3 p_end_tangent, const float p_end_tilt);

private:
	Ref<Curve3D> curve;
	Ref<Mesh> mesh;
	bool cubic;

	bool b_init;

private:
	Ref<Mesh> update_mesh();
};