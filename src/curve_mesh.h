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

	void set_mesh(const Ref<Mesh> &p_mesh);
	void set_use_tilt(bool p_use_tilt);
	void set_cubic(bool p_cubic);

	Ref<Mesh> set_start_end(Vector3 p_start_pos, Vector3 p_start_tangent, Vector3 p_end_pos, Vector3 p_end_tangent);
	Ref<Mesh> set_tilt(float p_start_tilt, float p_end_tilt);

private:
	Ref<Curve3D> curve = nullptr;
	Ref<Mesh> mesh = nullptr;
	bool cubic = true;
	bool use_tilt = true;

private:
	Ref<Mesh> update_mesh();
};