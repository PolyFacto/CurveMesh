@tool
extends Node3D

@export var mesh_source : Mesh
@onready var mesh_instance_3d: MeshInstance3D = $MeshInstance3D
@onready var path_3d: Path3D = $Path3D

var mesh : CurveMesh

func _ready():
	mesh = CurveMesh.new()
	mesh.init(mesh_source, true, path_3d.curve.up_vector_enabled)
	
	path_3d.connect("curve_changed", curve_changed)

func curve_changed() -> void:
	mesh_instance_3d.mesh = \
	mesh.create_curve_mesh(path_3d.curve.get_point_position(0), path_3d.curve.get_point_out(0), path_3d.curve.get_point_tilt(0),\
	 path_3d.curve.get_point_position(1), path_3d.curve.get_point_in(1), path_3d.curve.get_point_tilt(1))
