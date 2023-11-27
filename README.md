# HD

修改 `vendor/libpcap/pcap-linux.c` :

```diff
	if (handle->opt.buffer_size == 0) {
		/* by default request 2M for the ring buffer */
	-	handle->opt.buffer_size = 2*1024*1024;
	+	handle->opt.buffer_size = 100*1024*1024;
	}
```

```shell
mkdir build 
cd build 
cmake -DBUILD_MODE:STRING=LIVE_MODE,DEAD_MODE,INCLUDE_KAFKA,HD_DEV,BENCHMARK ..
```