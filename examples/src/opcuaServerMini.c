#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "opcua.h"
#include "ua_transport.h"
#include "ua_transport_binary.h"
#include "networklayer.h"

#ifdef LINUX

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/socketvar.h>
#include <unistd.h>

void server_run();

#endif

#define PORT 16664
#define MAXMSG 512
#define BUFFER_SIZE 8192

int main(void) {

#ifdef LINUX
	printf("Starting open62541 demo server on port %d\n", PORT);
	server_run();
#endif

	return EXIT_SUCCESS;
}

#ifdef LINUX

UA_Byte ack_msg_buf[] = { 					0x41, 0x43, /*       AC */
		0x4b, 0x46, 0x1c, 0x00, 0x00, 0x00, 0xff, 0xff, /* KF...... */
		0xff, 0xff, 0x00, 0x20, 0x00, 0x00, 0x00, 0x20, /* ... ...  */
		0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x01, 0x00, /* ...@.... */
		0x00, 0x00                                      /* ..       */
};
UA_ByteString ack_msg = { sizeof(ack_msg_buf), ack_msg_buf };
UA_ByteString* ack_msg_gb[] = { &ack_msg };


UA_Byte opn_msg_buf[] = {           0x4f, 0x50, /*       OP */
0x4e, 0x46, 0x88, 0x00, 0x00, 0x00, 0x19, 0x00, /* NF...... */
0x00, 0x00, 0x2f, 0x00, 0x00, 0x00, 0x68, 0x74, /* ../...ht */
0x74, 0x70, 0x3a, 0x2f, 0x2f, 0x6f, 0x70, 0x63, /* tp://opc */
0x66, 0x6f, 0x75, 0x6e, 0x64, 0x61, 0x74, 0x69, /* foundati */
0x6f, 0x6e, 0x2e, 0x6f, 0x72, 0x67, 0x2f, 0x55, /* on.org/U */
0x41, 0x2f, 0x53, 0x65, 0x63, 0x75, 0x72, 0x69, /* A/Securi */
0x74, 0x79, 0x50, 0x6f, 0x6c, 0x69, 0x63, 0x79, /* tyPolicy */
0x23, 0x4e, 0x6f, 0x6e, 0x65, 0xff, 0xff, 0xff, /* #None... */
0xff, 0xff, 0xff, 0xff, 0xff, 0x33, 0x00, 0x00, /* .....3.. */
0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0xc1, /* ........ */
0x01, 0x10, 0x56, 0x73, 0x9e, 0xdf, 0x63, 0xcf, /* ..Vs..c. */
0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
0x00, 0xff, 0xff, 0xff, 0xff, 0x19, 0x00, 0x00, /* ........ */
0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
0x00, 0x01, 0x00, 0x00, 0x00, 0x48              /* .....H   */
};

UA_ByteString opn_msg = { sizeof(opn_msg_buf), opn_msg_buf };
UA_ByteString* opn_msg_gb[] = { &opn_msg };

UA_Byte scm_msg_buf[] = {           0x4d, 0x53, /*       MS */
0x47, 0x46, 0x8b, 0x01, 0x00, 0x00, 0x19, 0x00, /* GF<XX>.. */ // <XX> = msglen. TODO: should be set or checked by writer?
0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x34, 0x00, /* ......4. */
0x00, 0x00, 0x02, 0x00, 0x00, 0x00              /* ......   */
};
UA_Byte rsp_msg_buf[] = {           0x01, 0x00, /* ........ */
0xaf, 0x01, 0x2c, 0xc1, 0x73, 0x9e, 0xdf, 0x63, /* <>,.s..c */ // <> = response nS0-ID
0xcf, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
0x00, 0x00};

UA_Byte gep_msg_buf[] = {
		    0x01, 0x00, 0x00, 0x00, 0x1b, 0x00, /*   ...... */
0x00, 0x00, 0x6f, 0x70, 0x63, 0x2e, 0x74, 0x63, /* ..opc.tc */
0x70, 0x3a, 0x2f, 0x2f, 0x31, 0x30, 0x2e, 0x30, /* p://10.0 */
0x2e, 0x35, 0x33, 0x2e, 0x31, 0x39, 0x38, 0x3a, /* .53.198: */
0x31, 0x36, 0x36, 0x36, 0x34, 0x27, 0x00, 0x00, /* 16664'.. */
0x00, 0x68, 0x74, 0x74, 0x70, 0x3a, 0x2f, 0x2f, /* .http:// */
0x6f, 0x70, 0x65, 0x6e, 0x36, 0x32, 0x35, 0x34, /* open6254 */
0x31, 0x2e, 0x69, 0x6e, 0x66, 0x6f, 0x2f, 0x61, /* 1.info/a */
0x70, 0x70, 0x6c, 0x69, 0x63, 0x61, 0x74, 0x69, /* pplicati */
0x6f, 0x6e, 0x73, 0x2f, 0x34, 0x37, 0x31, 0x31, /* ons/4711 */
0x25, 0x00, 0x00, 0x00, 0x68, 0x74, 0x74, 0x70, /* %...http */
0x3a, 0x2f, 0x2f, 0x6f, 0x70, 0x65, 0x6e, 0x36, /* ://open6 */
0x32, 0x35, 0x34, 0x31, 0x2e, 0x69, 0x6e, 0x66, /* 2541.inf */
0x6f, 0x2f, 0x70, 0x72, 0x6f, 0x64, 0x75, 0x63, /* o/produc */
0x74, 0x2f, 0x72, 0x65, 0x6c, 0x65, 0x61, 0x73, /* t/releas */
0x65, 0x03, 0x02, 0x00, 0x00, 0x00, 0x45, 0x4e, /* e.....EN */
0x19, 0x00, 0x00, 0x00, 0x54, 0x68, 0x65, 0x20, /* ....The  */
0x6f, 0x70, 0x65, 0x6e, 0x36, 0x32, 0x35, 0x34, /* open6254 */
0x31, 0x20, 0x61, 0x70, 0x70, 0x6c, 0x69, 0x63, /* 1 applic */
0x61, 0x74, 0x69, 0x6f, 0x6e, 0x00, 0x00, 0x00, /* ation... */
0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, /* ........ */
0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, /* ........ */
0xff, 0x01, 0x00, 0x00, 0x00, 0x2f, 0x00, 0x00, /* ...../.. */
0x00, 0x68, 0x74, 0x74, 0x70, 0x3a, 0x2f, 0x2f, /* .http:// */
0x6f, 0x70, 0x63, 0x66, 0x6f, 0x75, 0x6e, 0x64, /* opcfound */
0x61, 0x74, 0x69, 0x6f, 0x6e, 0x2e, 0x6f, 0x72, /* ation.or */
0x67, 0x2f, 0x55, 0x41, 0x2f, 0x53, 0x65, 0x63, /* g/UA/Sec */
0x75, 0x72, 0x69, 0x74, 0x79, 0x50, 0x6f, 0x6c, /* urityPol */
0x69, 0x63, 0x79, 0x23, 0x4e, 0x6f, 0x6e, 0x65, /* icy#None */
0x01, 0x00, 0x00, 0x00, 0x13, 0x00, 0x00, 0x00, /* ........ */
0x6d, 0x79, 0x2d, 0x61, 0x6e, 0x6f, 0x6e, 0x79, /* my-anony */
0x6d, 0x6f, 0x75, 0x73, 0x2d, 0x70, 0x6f, 0x6c, /* mous-pol */
0x69, 0x63, 0x79, 0x00, 0x00, 0x00, 0x00, 0xff, /* icy..... */
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, /* ........ */
0xff, 0xff, 0xff, 0x41, 0x00, 0x00, 0x00, 0x68, /* ...A...h */
0x74, 0x74, 0x70, 0x3a, 0x2f, 0x2f, 0x6f, 0x70, /* ttp://op */
0x63, 0x66, 0x6f, 0x75, 0x6e, 0x64, 0x61, 0x74, /* cfoundat */
0x69, 0x6f, 0x6e, 0x2e, 0x6f, 0x72, 0x67, 0x2f, /* ion.org/ */
0x55, 0x41, 0x2d, 0x50, 0x72, 0x6f, 0x66, 0x69, /* UA-Profi */
0x6c, 0x65, 0x2f, 0x54, 0x72, 0x61, 0x6e, 0x73, /* le/Trans */
0x70, 0x6f, 0x72, 0x74, 0x2f, 0x75, 0x61, 0x74, /* port/uat */
0x63, 0x70, 0x2d, 0x75, 0x61, 0x73, 0x63, 0x2d, /* cp-uasc- */
0x75, 0x61, 0x62, 0x69, 0x6e, 0x61, 0x72, 0x79, /* uabinary */
0x00                                            /* . */
};
UA_ByteString scm_msg = { sizeof(scm_msg_buf), scm_msg_buf };
UA_ByteString rsp_msg = { sizeof(rsp_msg_buf), rsp_msg_buf };
UA_ByteString gep_msg = { sizeof(gep_msg_buf), gep_msg_buf };
UA_ByteString* gep_msg_gb[] = { &scm_msg, &rsp_msg, &gep_msg };

UA_Int32 myProcess(TL_Connection* connection, const UA_ByteString* msg) {
	switch (*((UA_Int32*) &(msg->data[0]))) { // compare first four bytes
		case 0x464c4548: // HELF
			connection->writerCallback(connection, (const UA_ByteString**) &ack_msg_gb, 1);
		break;
		case 0x464e504f: // OPNF
			connection->writerCallback(connection, (const UA_ByteString**) &opn_msg_gb, 1);
		break;
		case 0x464f4c43: // CLOF
			connection->connectionState = CONNECTIONSTATE_CLOSE;
		break;
		case 0x4647534d: // MSGF
			switch (*((UA_Int16*) &(msg->data[26]))) {
				case (UA_Int16) 0xAC01: // 428 - GetEndpointsRequest
					printf("server_run - GetEndpointsRequest\n");
					connection->writerCallback(connection, (const UA_ByteString**) &gep_msg_gb, 3);
				break;
				default:
					//FIXME: construct and send a real error message
					connection->writerCallback(connection, (const UA_ByteString**) &gep_msg_gb, 3);
				break;
			}
		break;
	}
	return UA_SUCCESS;
}
void server_run() {
	TL_Connection connection;
	connection.connectionState = CONNECTIONSTATE_CLOSED;
	connection.writerCallback = (TL_Writer) NL_TCP_writer;
	connection.localConf.maxChunkCount = 1;
	connection.localConf.maxMessageSize = BUFFER_SIZE;
	connection.localConf.protocolVersion = 0;
	connection.localConf.recvBufferSize = BUFFER_SIZE;
	connection.localConf.recvBufferSize = BUFFER_SIZE;

	UA_ByteString slMessage = { -1, UA_NULL };

	int optval = 1;
	int sockfd, newsockfd, portno, clilen;
	char buffer[BUFFER_SIZE];
	struct sockaddr_in serv_addr, cli_addr;
	int n;

	/* First call to socket() function */
	sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("ERROR opening socket");
		exit(1);
	}

	/* Initialize socket structure */
    memset((void *) &serv_addr, 0, sizeof(serv_addr));
	portno = PORT;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval) == -1) {
		perror("setsockopt");
		exit(1);
	}

	/* Now bind the host address using bind() call.*/
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		perror("ERROR on binding");
		exit(1);
	}

	/* Now start listening for the clients, here process will
	 * go in sleep mode and will wait for the incoming connection
	 */
	while (listen(sockfd, 5) != -1) {
		clilen = sizeof(cli_addr);
		/* Accept actual connection from the client */
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t*) &clilen);
		if (newsockfd < 0) {
			perror("ERROR on accept");
			exit(1);
		}

		printf("server_run - connection accepted: %i, state: %i\n", newsockfd, connection.connectionState);
		connection.connectionHandle = newsockfd;
		do {
            memset(buffer, 0, BUFFER_SIZE);
			n = read(newsockfd, buffer, BUFFER_SIZE);
			if (n > 0) {
                slMessage.data = (UA_Byte*) buffer;
				slMessage.length = n;
				UA_ByteString_printx("server_run - received=",&slMessage);
				myProcess(&connection, &slMessage);
			} else if (n <= 0) {
				perror("ERROR reading from socket1");
				connection.connectionState = CONNECTIONSTATE_CLOSE;
			}
		} while(connection.connectionState != CONNECTIONSTATE_CLOSE);
		shutdown(newsockfd,2);
		close(newsockfd);
		connection.connectionState = CONNECTIONSTATE_CLOSED;
	}
	shutdown(sockfd,2);
	close(sockfd);
}

#endif
