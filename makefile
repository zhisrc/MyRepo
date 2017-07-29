ymsg: client_analysis.o geturl.o chunk.o http_hash.o http.o  keyword_match.o tcp_block.o logqueue.o conn.o tcp_queue.o global.o update_config.o associate.o control_hash.o data_hash.o ftp_other_func.o main.o tcp_hash.o telnet.o telnet_link.o telnet_log.o write_orcl_circular_queue.o ymsg_log.o
	gcc -g -o  $@ $^ -lnids -lnet -lpcap  -lz -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -I${ORACLE_HOME}/sdk/instantclient_12_1/sdk/include -I${ORACLE_HOME}/sdk/instantclient_12_1/sdk/demo -L${ORACLE_HOME}/lib -lclntsh -lpthread


%.o: %.c
	gcc -g -c -o $@ $< -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -I${ORACLE_HOME}/sdk/instantclient_12_1/sdk/include -I${ORACLE_HOME}/sdk/instantclient_12_1/sdk/demo -L${ORACLE_HOME}/lib -lclntsh -lpthread -lnids -lnet -lpcap  -lz
