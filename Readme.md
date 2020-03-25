```多线程版live555

## 缘由:

众所周知,live555的设计是单线程的,由TaskScheduler调度每个单步动作。我目前遇到的实际问题如下：
我这嵌入式设备要发16路视频流。如果有一个客户端A用tcp连接，但因为网络不畅，会导致服务端发送来不及，但除了A，剩下的客户端都是网络状况好的。 这个A的延迟会导致其余的客户端都被拖累。发送一帧60k的数据可能耗时400毫秒， 当前客户端来不及发送，没关系。但不能影响其他客户端。



### 修改方法

![](C:\Users\lenovo\Pictures\TIM图片20200325202918.png)

原生的请求关系如上图, RtspServer将accept后的socket交给ClientConnection(以下简称conn)，由conn监听http并且处理，RtspServer中保存了conn列表，ServerMediaSession列表，和ClientSession列表，并且在后续的http请求中又调用RtspServer将自己从链表中删除。这几方调用关系错综复杂。如果要修改将触及live555大动脉。

我的修改是

1、为每一次rtsp请求建立RTSPServer

2、RTSPServer将不再accept，而是由主线程accept，
3、在accpet后将socket给单独的线程,并且在线程中分配RTSPServer,TaskScheduler和UsageEnvironment
为此需要修改GenericMediaServer，让其不再accept，只监听主线程传过来的socket，此后的处理照旧。



### 难点:何时关闭发送子线程?

rtsp请求可能包含http连接，rtp连接，原生live555设计中关闭Http连接并不一定会停止发送rtp，但本例中的修改是http断开,立刻停止rtp发送。



### 优点

1. 在独立的线程中给客户端发送视频流,有一个客户端阻塞了发送不会影响到其他客户端

2. 用不到50行的代码修改带来发送性能的巨大提升

3. 代码修改风格沿用live555

4. 线程采用c++11 std::thread,不会带来额外部署开销

   

### 缺点

1. 失去live555重用source的特性, 在单线程模式中,多个客户端有可能共用一个媒体源。比如RTPSource，现在的修改会造成所有的source都是在线程局部变量中保存，无法共享。如果要解决需要规划你的服务，将要重用的source放在同一个线程中，且线程中的source长期存在。

2. 关闭子线程的策略不够完善。有待后续解决

   

### 使用方法

1. windows上使用vs2017打开工程并编译运行

2. ./ffmpeg.exe -i rtsp://127.0.0.1/ch0.liv

   

   

   

