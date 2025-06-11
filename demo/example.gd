@tool
extends Node3D

@export var mesh_source : Mesh
@onready var mesh_instance_3d: MeshInstance3D = $MeshInstance3D
@onready var path_3d: Path3D = $Path3D

var mesh : CurveMesh = CurveMesh.new()

func _ready():
	mesh.set_mesh(mesh_source)
	mesh.set_cubic(true)
	mesh.set_use_tilt(true)
	path_3d.connect("curve_changed", curve_changed)

func curve_changed() -> void:
	var deformed_mesh: ArrayMesh = null
	deformed_mesh = mesh.set_start_end(path_3d.curve.get_point_position(0), path_3d.curve.get_point_out(0), path_3d.curve.get_point_position(1), path_3d.curve.get_point_in(1))
	deformed_mesh = mesh.set_tilt(path_3d.curve.get_point_tilt(0), path_3d.curve.get_point_tilt(1))
	mesh_instance_3d.mesh = deformed_mesh
