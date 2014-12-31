cam_cal: cam_cal.cpp
	g++-4.4 -g -I/home/shared/software/include -D_x64 -D_LINUX cam_cal.cpp -o cam_cal -lPvAPI

camera_test: camera_test.cpp
	g++-4.4 -g -I/home/shared/software/include -D_x64 -D_LINUX camera_test.cpp -o camera_test -lPvAPI

camera_test_arm: camera_test_arm.o
	g++-4.4 -g -I/home/shared/software/include -D_arm -D_LINUX camera_test_arm.o libPvAPI_arm.a -o camera_test_arm

camera_test_arm.o: camera_test.cpp
	g++-4.4 -g -I/home/shared/software/include -D_arm -D_LINUX -c camera_test.cpp -o camera_test_arm.o

camera_test_x64: camera_test_x64.o
	g++-4.4 -g -I/home/shared/software/include -D_x64 -D_LINUX camera_test_x64.o -o camera_test_x64 -L. -lPvAPI_x64 -lpthread -lrt

camera_test_x64.o: camera_test.cpp
	g++ -g -I/home/shared/software/include -D_x64 -D_LINUX -c camera_test.cpp -o camera_test_x64.o

a35_x64: a35_x64.o
	g++ -g -I/home/shared/software/include -D_x64 -D_LINUX a35_x64.o -o a35_x64 -L. -lPvAPI_x64 -lpthread -lrt

a35_x64.o: a35.cpp
	g++ -g -I/home/shared/software/include -D_x64 -D_LINUX -c a35.cpp -o a35_x64.o
