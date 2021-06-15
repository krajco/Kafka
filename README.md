# Kafka
Monitoring tool that displays information about network flows on STDOUT. The tool was originally linked to apache Kafka, which served as a broker. These features have been removed. 

The network flow consists of five values:
- source ip address
- destination ip address
- source port number
- destination port number
- protokol type

## Features
- Each network flow consists of two parts. The first part contains information about the flow. The second part consists of a pair of timestamp and payload sizes of the first twenty packets
- Network flows are checked every twenty seconds. Network flows that are inactive are exported. 
- The program detects the TCP fin flag. When the network flow is completed, it is exported.
- The program captures the ctrl + c signal. When this signal is detected, the cycle is stopped and the remaining network flows are exported.



## Installation
```sh
make
sudo ./producer
```
