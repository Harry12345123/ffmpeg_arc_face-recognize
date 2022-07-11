（1）博主原文：
注意：在运行程序之间的准备工作，首先要进行Opencv的安装(最好是opencv3.x版本)、FFMPEG的安装。然后把虹软的动态库加载进来(libarcsoft_face.so和libarcsoft_face_engine.so),还有sqlite3数据库的安装。Makefile的内容也需要根据自行安装的Opencv、FFMPEG、虹软的安装路径进行修改。


第一步：首先运行asfort_face_insert的程序，这个程序的主要功能是读取一张人脸图片。并把人脸图片的特征读取出来，然后把人脸特征存储到数据库里面。具体的运行程序参数如图：
./asfort_face_insert ./face_lib/face01.png
其中face_path是人脸图片的路径，如./face_lib/face01.png
face_name是录入人脸的名称，如:Harry，James

当打印日志出现Get Face Feature Success则代表插入成功，此时人脸数据库就有了数据。
(小提示：这里的人脸图片最好利用电脑自带的摄像头进行拍照，或者用自己的一个正面照即可)。


第二步：现在我们的数据库已经有了人脸数据，那下一步我们直接运行我们的主程序：
ffmpeg_camera_asfort(运行方式：./ ffmpeg_camera_asfort)，运行成功的话会看到，一个电脑自带的摄像头出现，并且会把自己的名字显示在一个矩形框里面
这里要注意的是，在运行ffmpeg_camera_asfort的时候，一定要在Linux虚拟机里面把摄像头打开，打开方式：
首选项->可移动设备->选择摄像头设备的名称(这里的摄像头名称各有不同，以自己电脑的名称为主，如我的摄像头名称是：IMC Networks Integrated Camera)->再选择连接。此时在虚拟机里面摄像头就已经打开了。这一步非常重要，如果没有这一步，程序直接报错。

（2）源码安装包分享（群友：请回答2022 时间：20220616）
大家安装环境时（ffmpeg，opencv，sqlite）可以从这个百度云链接里下载源码安装包，版本可能略有不同，但大差不差，都能用。
链接：https://pan.baidu.com/s/1NfUVzWx8tsa8dKpHL4SI2g 
提取码：qyn1
另外，ffmpeg的安装过程可以参考这个链接http://blog.yundiantech.com/?log=blog&id=35

（3）群友个人运行分享（群友：请回答2022 时间：20220616）
    1）群主的makefile里面有几个lib引用可以删掉，否则可能因为没安装相应软件报错，如libasound，libsdl，本项目主要使用ffmpeg opencv sqlite 虹软，其他lib用不到
    2）我在编译完，使用./asfort_face_insert时仍然显示未找到虹软的so文件，我使用export LD_LIBRARY_PATH = /xxx/xxx，将LD_LIBRARY_PATH指向虹软的so文件才能继续正常运行。我也不知道具体原因，在makefile里已经指定这文件夹了，为什么还需要export LD_LIBRARY_PATH
    3）博主第一步的命令是./asfort_face_insert ./face_lib/face01.png Harry
    4）如果运行程序出现ASFInitEngine fail：90115，不要慌张，正常现象，虹软sdk要求在使用前进行验证。我直接在虹软官网注册了一下，重新获取了sdk。参考虹软sdk的说明书需要在使用前调用Active程序激活，具体函数及说明详见说明书。另外记得把博主程序里的设备id和激活码改成你自己的。
