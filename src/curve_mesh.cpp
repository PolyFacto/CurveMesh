#include "curve_mesh.h"

#include <godot_cpp/classes/mesh_data_tool.hpp>
#include <godot_cpp/core/math.hpp>
#include <limits>

CurveMesh::CurveMesh()
{
    curve.instantiate();
}

void CurveMesh::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("set_mesh", "mesh"), &CurveMesh::set_mesh);
    ClassDB::bind_method(D_METHOD("set_cubic", "cubic"), &CurveMesh::set_cubic);
    ClassDB::bind_method(D_METHOD("set_use_tilt", "use_tilt"), &CurveMesh::set_use_tilt);
    ClassDB::bind_method(D_METHOD("set_start_end", "start_position", "start_tangent", "end_position", "end_tangent"), &CurveMesh::set_start_end);
    ClassDB::bind_method(D_METHOD("set_tilt", "start_tilt", "end_tilt"), &CurveMesh::set_tilt);
}

void CurveMesh::set_mesh(const Ref<Mesh> &p_mesh)
{
    mesh = p_mesh;
}

void CurveMesh::set_use_tilt(bool p_use_tilt)
{
    use_tilt = p_use_tilt;
}

void CurveMesh::set_cubic(bool p_cubic)
{
    cubic = p_cubic;
}

Ref<Mesh> CurveMesh::set_start_end(Vector3 p_start_pos, Vector3 p_start_tangent, Vector3 p_end_pos, Vector3 p_end_tangent)
{
    if (curve.is_null()) return nullptr;
    if (mesh.is_null())
    {
        UtilityFunctions::printerr("Please set a mesh first.");
        return nullptr;
    }

    if (curve->get_point_count() == 0)
    {
        curve->add_point(p_start_pos, Vector3(0.0f, 0.0f, 0.0f), p_start_tangent);
        curve->add_point(p_end_pos, p_end_tangent, Vector3(0.0f, 0.0f, 0.0f));
    }

    curve->set_point_position(0, p_start_pos);
    curve->set_point_out(0, p_start_tangent);
    curve->set_point_position(1, p_end_pos);
    curve->set_point_in(1, p_end_tangent);

    return update_mesh();
}

Ref<Mesh> CurveMesh::set_tilt(float p_start_tilt, float p_end_tilt)
{
    if (curve.is_null()) return nullptr;
    if (mesh.is_null())
    {
        UtilityFunctions::printerr("Please set a mesh first.");
        return nullptr;
    }

    curve->set_point_tilt(0, p_start_tilt);
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
        UtilityFunctions::printerr("Failed to read mesh surface.");
        return nullptr;
    }

    float min_z = std::numeric_limits<float>::infinity();
    float max_z = -std::numeric_limits<float>::infinity();

    for (int i = 0; i < mdt->get_vertex_count(); i++)
    {
        float z = mdt->get_vertex(i).z;
        min_z = Math::min(min_z, z);
        max_z = Math::max(max_z, z);
    }

    float mesh_length = max_z - min_z;

    for (int v = 0; v < mdt->get_vertex_count(); v++)
    {
        Vector3 vertex = mdt->get_vertex(v);
        float local_z = -vertex.z - max_z;
        float offset = (local_z / mesh_length) * curve_length;

        offset = Math::clamp(offset, 0.0f, curve_length);

        Transform3D transform = curve->sample_baked_with_rotation(offset, cubic, use_tilt); // TODO : Pass cubic and tilt parameter
        Vector3 local_vertex = Vector3(vertex.x, vertex.y, 0.0);
        auto rotated_vertex = transform.basis.xform(local_vertex);
        auto final_position = transform.origin + rotated_vertex;

        mdt->set_vertex(v, final_position);
    }

    mdt->commit_to_surface(current_deformed_mesh);
    return current_deformed_mesh;
}