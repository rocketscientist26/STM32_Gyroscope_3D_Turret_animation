extends Spatial

var gyro = Gyroscope.new()
var gX = 0
var gZ = 0
var gY = 0

var h_rot_vec = Vector3(0,0,0)
var v_rot_vec  = Vector3(0,0,0)

func _ready():
	# send reset here
	gyro.gyroscope_init()

func _on_Timer_timeout():
	gX = gyro.gyroscope_get_X()
	gX = gX + 90
	if(gX>360):
		gX -= 360
	gZ = gyro.gyroscope_get_z()
	gZ = gZ + 180
	if(gZ>360):
		gZ -= 360
	gY = gyro.gyroscope_get_y()
	v_rot_vec = Vector3(gX,0,0)
	h_rot_vec = Vector3(0,gZ,0)
	get_node("Turret/Gun").set_rotation_deg(v_rot_vec)
	get_node("Turret").set_rotation_deg(h_rot_vec)
	get_node("Timer").start()

func _on_Button_pressed():
	gyro.gyroscope_reset()