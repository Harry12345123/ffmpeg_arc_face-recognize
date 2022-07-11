all:
	g++ map_manage.cpp sqlite3_operation.cpp asfort_manage.cpp opencv_queue.cpp ffmpeg_camera_asfort.cpp -I/usr/local/ffmpeg4.1/include -L/usr/local/ffmpeg4.1/lib -o ffmpeg_camera_asfort -lsqlite3 -lSDL2 -lSDL -lm -lasound -larcsoft_face_engine -larcsoft_face -lavformat -lavcodec -lavutil -lavfilter -lavdevice -lswresample -lswscale -lpthread -lopencv_core -lopencv_dnn -lopencv_features2d -lopencv_flann -lopencv_highgui -lopencv_imgcodecs -lopencv_imgcodecs -lopencv_imgproc -lopencv_ml -lopencv_objdetect -lopencv_photo -lopencv_shape -lopencv_stitching -lopencv_superres -lopencv_videoio -lopencv_video -larcsoft_face_engine -larcsoft_face
	g++ asfort_face_insert.cpp sqlite3_operation.cpp asfort_manage.cpp -I/usr/local/ffmpeg4.1/include -L/usr/local/ffmpeg4.1/lib -o asfort_face_insert -lsqlite3 -lSDL2 -lSDL -lm -lasound -larcsoft_face_engine -larcsoft_face -lavformat -lavcodec -lavutil -lavfilter -lavdevice -lswresample -lswscale -lpthread -lopencv_core -lopencv_dnn -lopencv_features2d -lopencv_flann -lopencv_highgui -lopencv_imgcodecs -lopencv_imgcodecs -lopencv_imgproc -lopencv_ml -lopencv_objdetect -lopencv_photo -lopencv_shape -lopencv_stitching -lopencv_superres -lopencv_videoio -lopencv_video -larcsoft_face_engine -larcsoft_face
