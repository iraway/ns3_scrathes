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