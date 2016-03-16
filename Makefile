kr_6050_control.bin: I2Cdev4Edison.cpp MPU6050_4Edison.cpp MPU6050_6Axis_MotionApps20_4Edison.cpp demo_optimized4Edison.cpp helper_3dmath4Edison.cpp
	g++ I2Cdev4Edison.cpp MPU6050_4Edison.cpp MPU6050_6Axis_MotionApps20_4Edison.cpp demo_optimized4Edison.cpp helper_3dmath4Edison.cpp -lmraa -o $@
