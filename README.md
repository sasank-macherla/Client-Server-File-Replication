# Client-Server-File-Replication
This project deals with client and server daemons to replicate multiple folders of client on the server daemon along with many more features.

1) A client daemon is configured with a directory to be monitored and has the capability of restoring the last saved copy from server daemon and also they deal with the consistency issue.

2) The server daemon can replicate multiple files. The client and server daemons are located on same hosts or different ones.

3) Different directories for each daemon is configured to work on, so they do not interfere with each other when they are running on same host.

4) The system can store replicas encrypted (DES Encryption)

