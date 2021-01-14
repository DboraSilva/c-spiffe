/*
 * Filename: /home/rlc2/Documents/c-spiffe/workload/src/client.c
 * Path: /home/rlc2/Documents/c-spiffe/workload/src
 * Created Date: Tuesday, December 22nd 2020, 1:22:45 pm
 * Author: Rodrigo Lopes (rlc2@cesar.org.br)
 * 
 * Copyright (c) 2020 CESAR
 */

// #include "proto/workload.pb.h"
// #include "proto/workload.grpc.pb.h"
#include "requestor.h"
#include <cstdio>
#include <cstdlib>
int main(int argc, char const *argv[])
{
    x509svid_SVID* svid = fetch_SVID();
    printf("id:%s\n%s\n%s\n%s\n",svid->id.path,svid->id.td,svid->certs,svid->privateKey);
    return 0;
}