/**
 * Forward and reverse DNS queries that use the POSIX builtin name resolver.
 *
 * Copyright 2023 Alexandre Emsenhuber
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

const char* proto_desc( int protocol ) {
	if ( protocol == 0 ) return "RAW";
	if ( protocol == IPPROTO_ICMP ) return "ICMP";
	if ( protocol == IPPROTO_TCP ) return "TCP";
	if ( protocol == IPPROTO_UDP ) return "UDP";
	return "UNKNOWN";
}

int main( int argc, char** argv ) {
	int help = 0;

	/* Prepare options right away so that they can be directly changed when parsing the command line */
	struct addrinfo hints;
	memset( &hints, '\0', sizeof( hints ) );

	hints.ai_family = AF_UNSPEC;     /* Allow IPv4 or IPv6 */
	hints.ai_flags = AI_CANONNAME;   /* Include canonical name */

	/* Parse command line */
	/* ------------------ */

	while ( 1 ) {
		int opt = getopt( argc, argv, "h46tu" );
		if ( opt < 0 ) break;
		if ( opt == '?' ) exit( 2 );
		if ( opt == 'h' ) help++;
		if ( opt == '4' ) hints.ai_family = AF_INET;
		if ( opt == '6' ) hints.ai_family = AF_INET6;
		if ( opt == 't' ) hints.ai_socktype = SOCK_STREAM;
		if ( opt == 'u' ) hints.ai_socktype = SOCK_DGRAM;
	}

	if ( help || ( argc - optind != 1 && argc - optind != 2 ) ) {
		fprintf( stderr, "Usage: %s [options] host-or-address [service]\n", argv[0] );

		if ( help ) {
			fprintf( stderr, "\n" );
			fprintf( stderr, "Options:\n" );
			fprintf( stderr, "-h  Display this help message\n" );
			fprintf( stderr, "-4  Query only IPv4 addresses\n" );
			fprintf( stderr, "-6  Query only IPv6 addresses\n" );
			fprintf( stderr, "-t  Query only TCP serivces\n" );
			fprintf( stderr, "-u  Query only UDP services\n" );

			exit( EXIT_SUCCESS );
		} else {
			exit( 2 );
		}
	}

	const char* in_host = argv[optind];
	const char* in_serv = ( argc - optind >= 2 ? argv[optind + 1] : NULL );

	/* Perform forward query */
	/* --------------------- */

	struct addrinfo* res;
	int ret = getaddrinfo( in_host, in_serv, &hints, &res );
	if ( ret != 0 ) {
		fprintf( stderr, "getaddrinfo: %s\n", gai_strerror( ret ) );
		exit( EXIT_FAILURE );
	}

	/* With AI_CANONNAME flag, only the first result has the "ai_canonname" field set. */
	if ( res->ai_canonname != NULL && strcasecmp( res->ai_canonname, in_host ) ) {
		printf( "Canonical name %s\n", res->ai_canonname );
	}

	struct addrinfo* item;
	for ( item = res; item != NULL; item = item->ai_next ) {
		if ( in_serv == NULL && item->ai_protocol != 0 ) continue;

		if ( item->ai_family == AF_INET ) {
			struct sockaddr_in* in_addr = (struct sockaddr_in*) item->ai_addr;
			char dst[257];
			inet_ntop( AF_INET, &( in_addr->sin_addr ), dst, 256 );
			printf( "IPv4 address %s", dst );
			if ( in_serv != NULL ) {
				printf( ", %s port %d", proto_desc( item->ai_protocol ), htons( in_addr->sin_port ) );
			}

			char rev_host[NI_MAXHOST];
			char rev_serv[NI_MAXSERV];
			int rev_res = getnameinfo( item->ai_addr, item->ai_addrlen, rev_host, NI_MAXHOST, rev_serv, NI_MAXSERV, item->ai_protocol == IPPROTO_UDP ? NI_DGRAM : 0 );
			if ( rev_res >= 0 ) {
				if ( in_serv == NULL ) {
					printf( ", reverse %s", rev_host );
				} else {
					printf( ", reverse name %s service %s", rev_host, rev_serv );
				}
			}
			printf( "\n" );
		} else if ( item->ai_family == AF_INET6 ) {
			struct sockaddr_in6* in6_addr = (struct sockaddr_in6*) item->ai_addr;
			char dst[257];
			inet_ntop( AF_INET6, &( in6_addr->sin6_addr ), dst, 256 );
			printf( "IPv6 address %s", dst );
			if ( in_serv != NULL ) {
				printf( ", %s port %d", proto_desc( item->ai_protocol ), htons( in6_addr->sin6_port ) );
			}

			char rev_host[NI_MAXHOST];
			char rev_serv[NI_MAXSERV];
			int rev_res = getnameinfo( item->ai_addr, item->ai_addrlen, rev_host, NI_MAXHOST, rev_serv, NI_MAXSERV, item->ai_protocol == IPPROTO_UDP ? NI_DGRAM : 0 );
			if ( rev_res >= 0 ) {
				if ( in_serv == NULL ) {
					printf( ", reverse %s", rev_host );
				} else {
					printf( ", reverse name %s service %s", rev_host, rev_serv );
				}
			}
			printf( "\n" );
		} else {
			printf( "Unknown record type %d\n", item->ai_family );
		}
	}

	freeaddrinfo( res );

	return EXIT_SUCCESS;
}
