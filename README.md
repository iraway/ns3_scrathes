# ns3_scrathes

### How to run
You need to put projects in folder "../ns-allinone-3.33/ns-3.33/scratch" and then
```sh
$ cd ns-allinone-3.33/ns-3.33 
$ ./waf -- run <project_name>
```
If you want to see log outputs: 
```sh
$ NS_LOG="<project_logs>" ./waf --run <project_name> 
```


### List of projects:

1. SimpleUdpApplication \
Three nodes in one network. It's possible to send packets with custom data between them. 

2. SocketApplication \
Updated version of SimpleUdpApplication. Save PCAP filtes into ../ns-allinone-3.33/ns-3.33/pcap/SocketApplication path.

3. SocketApplicationUdpTcp \
In this version it's possible to use UDP or TCP protocol. In TCP case TcpSinkHelper class was used. To run application: 
```sh
$ ./waf -- run "SocketApplicationUdpTcp --udp={true/false}"
```

