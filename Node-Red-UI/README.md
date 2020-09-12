# MachineOverview

## Setup
follow this [link](https://docs.docker.com/engine/install/ubuntu/) to setup docker on Ubuntu.

pull image from docker hub, dashboard has been installed in this image 
```
docker pull liamchen00/mynodered:latest
```
run docker container. ```/home/cyy/nodered-file``` can be changed to your own directory 
```
docker run -it -p 1880:1880 -v /home/cyy/nodered-file:/data --name mynodered liamchen00/mynodered
```
ctrl+P+Q: quit and container will keep running in the background

copy flows.json file into folder " /home/cyy/nodered-file", and  restart docker container.
```
docker restart mynodered
```

## Running
basic commands:
 ```
 docker ps -a   // show all docker container
 docker  strart [container name]    // start a docker continer
 docker stop [container name]   // stop a docker container
 docker  rm [containner name or id] //delete a docker container
 docker docker exec -it [container] /bin/bash   //enter container Shell
 ```

 start container mynodered
 ```
 docker start mynodered
 ```
 You can then browse to http://127.0.0.1:1880/ to get Node-RED desktop. and to http://127.0.0.1:1880/ui/ to get dashboard.

## References
 https://nodered.org/docs/getting-started/docker#startup