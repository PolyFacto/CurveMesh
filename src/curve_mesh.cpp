#include "curve_mesh.h"

#include <godot_cpp/classes/mesh_data_tool.hpp>
#include <godot_cpp/core/math.hpp>
#include <limits>

CurveMesh::CurveMesh()
{
    curve.instantiate();
    b_init = false;
}

void CurveMesh::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("init", "mesh", "cubic"), &CurveMesh::init);
    ClassDB::bind_method(D_METHOD("create_curve_mesh", "start_position", "start_tangent", "start_tilt", "end_position", "end_tangent", "end_tilt"), &CurveMesh::create_curve_mesh);
}

void CurveMesh::init(const Ref<Mesh> &p_mesh, const bool p_cubic)
{
    mesh = p_mesh;
    cubic = p_cubic;

    b_init = true;
}

Ref<Mesh> CurveMesh::create_curve_mesh(const Vector3 p_start_pos, const Vector3 p_start_tangent, const float p_start_tilt, const Vector3 p_end_pos, const Vector3 p_end_tangent, const float p_end_tilt)
{

    if (curve.is_null()) return nullptr;
    if (!b_init)
    {
        print_error("Please call the init function before.");
        return nullptr;
    }

    if (curve->get_point_count() == 0)
    {
        curve->add_point(p_start_pos, Vector3(0.0f, 0.0f, 0.0f), p_start_tangent);
        curve->add_point(p_end_pos, p_end_tangent, Vector3(0.0f, 0.0f, 0.0f));
    }

    curve->set_point_position(0, p_start_pos);
    curve->set_point_out(0, p_start_tangent);
    curve->set_point_tilt(0, p_start_tilt);
    curve->set_point_position(1, p_end_pos);
    curve->set_point_in(1, p_end_tangent);
    curve->set_point_tilt(1, p_end_tilt);

    return update_mesh();
}

Ref<Mesh> CurveMesh::update_mesh()
{
    if (mesh.is_null()) return nullptr;

    float curve_length = curve->get_baked_length();

    Ref<ArrayMesh> current_deformed_mesh;
    current_deformed_mesh.instantiate();

    Ref<MeshDataTool> mdt;
    mdt.instantiate();
    Error err = mdt->create_from_surface(mesh, 0);
    if (err != OK) {
        print_error("Failed to read mesh surface.");
        return nullptr;
    }

    float min_proj = std::numeric_limits<float>::infinity();
    float max_proj = -std::numeric_limits<float>::infinity();

    Vector3 forward = Vector3(0.0f, 0.0f, -1.0f);

    for (int i = 0; i < mdt->get_vertex_count(); i++)
    {
        float proj = mdt->get_vertex(i).dot(forward);
        min_proj = Math::min(min_proj, proj);
        max_proj = Math::max(max_proj, proj);
    }

    float mesh_length = max_proj - min_proj;

    for (int v = 0; v < mdt->get_vertex_count(); v++)
    {
        Vector3 vertex = mdt->get_vertex(v);
        float proj = vertex.dot(forward);
        float local_z = proj - min_proj;
        float offset = (local_z / mesh_length) * curve_length;

        offset = Math::clamp(offset, 0.0f, curve_length);

        Transform3D transform = curve->sample_baked_with_rotation(offset, cubic, true);

        Vector3 local_vertex = vertex - forward * proj;
        auto rotated_vertex = transform.basis.xform(local_vertex);
        auto final_position = transform.origin + rotated_vertex;

        mdt->set_vertex(v, final_position);
    }

    mdt->commit_to_surface(current_deformed_mesh);
    return current_deformed_mesh;
}