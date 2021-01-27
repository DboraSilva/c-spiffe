/*
 * Filename: c-spiffe/requestor/requestor.cpp
 * Path: c-spiffe/requestor
 * Created Date: Monday, December 21nd 2020, 10:32:38 am
 * Author: Rodrigo Lopes (rlc2@cesar.org.br)
 * 
 * Copyright (c) 2020 CESAR
 */

#include "../src/requestor.h"
#include <check.h>
#include <grpc/grpc.h>
#include <grpcpp/grpcpp.h>
#include <gmock/gmock.h>
#include <grpcpp/test/mock_stream.h>
#include "workload.pb.h"
#include "workload.grpc.pb.h"
#include "workload_mock.grpc.pb.h"
#include "../../svid/x509svid/src/svid.h"
#include <cstring>
using grpc::testing::MockClientReader;

using ::testing::_;
using ::testing::AtLeast;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SaveArg;
using ::testing::SetArgPointee;
using ::testing::WithArg;


START_TEST(test_requestor_init)
{
    //normal constructor test
    const char* addr = "unix:///tmp/agent.sock";
    MockSpiffeWorkloadAPIStub* stub = new MockSpiffeWorkloadAPIStub();
    Requestor* reqtor = RequestorInitWithStub(addr,stub);
    
    ck_assert_ptr_ne(reqtor,NULL);
    ck_assert_ptr_ne(reqtor->address,NULL);
    ck_assert_ptr_ne(reqtor->address,addr);//has to be a copy, not the same string
    ck_assert_int_eq(strlen(reqtor->address),strlen(addr));
    ck_assert_str_eq(reqtor->address, addr);

    RequestorFree(reqtor);

    //NULL server address test
    
    reqtor = RequestorInit(NULL);

    ck_assert_ptr_eq(reqtor,NULL);
    
    //no need to free failed requestor

}
END_TEST

START_TEST(test_requestor_free)
{
    const char* addr = "unix:///tmp/agent.sock";
    Requestor* reqtor = RequestorInit(addr);
    
    ck_assert_ptr_ne(reqtor,NULL);
    ck_assert_ptr_ne(reqtor->address,NULL);
    RequestorFree(reqtor);
    //TODO add malloc and free counter

}
END_TEST

ACTION(set_single_SVID_response) {
    arg0->set_spiffe_id("spiffe://example.org/workload_test");
    arg0->set_x509_svid("X509BIN");
    arg0->set_x509_svid_key("X509KEYBIN");
}
ACTION(set_multi_SVID_response) {
    arg0->set_spiffe_id("spiffe://example.org/workload_test");
    arg0->set_x509_svid("MULTIX509BIN");
    arg0->set_x509_svid_key("X509KEYBIN");
}


START_TEST(test_fetch_default_x509)
{
    //mocks the ClientReader class so we can craft the responses
    auto cr = new MockClientReader<X509SVIDResponse>();
    MockSpiffeWorkloadAPIStub *mock_stub = new MockSpiffeWorkloadAPIStub();
    EXPECT_CALL(*cr, Read(_))
      .WillRepeatedly(DoAll(WithArg<0>(set_single_SVID_response),Return(true)));
    //   .WillOnce(Return(false));
    const char* addr = "unix:///tmp/agent.sock";
    Requestor* reqtor = RequestorInitWithStub(addr,(stub_ptr) mock_stub);
    EXPECT_CALL(*mock_stub, FetchX509SVIDRaw)
      .WillOnce(Return(cr));

    x509svid_SVID* _svid = FetchDefaultX509SVID(reqtor);

    ck_assert_ptr_ne(_svid,NULL);

    
    RequestorFree(reqtor);
    free(_svid);
}
END_TEST

START_TEST(test_fetch_all_x509)
{
        //mocks the ClientReader class so we can craft the responses
    auto cr = new MockClientReader<X509SVIDResponse>();
    MockSpiffeWorkloadAPIStub *mock_stub = new MockSpiffeWorkloadAPIStub();
    EXPECT_CALL(*cr, Read(_))
      .WillOnce(DoAll(WithArg<0>(set_single_SVID_response),Return(true)))
      .WillOnce(Return(false));
    const char* addr = "unix:///tmp/agent.sock";
    Requestor* reqtor = RequestorInitWithStub(addr,new MockSpiffeWorkloadAPIStub());
    EXPECT_CALL(*mock_stub, FetchX509SVIDRaw)
      .WillOnce(Return(cr));

    x509svid_SVID* _svid = FetchDefaultX509SVID(reqtor);

    ck_assert_ptr_ne(_svid,NULL);

    
    RequestorFree(reqtor);
    free(_svid);
}
END_TEST


Suite* requestor_suite(void)
{
    Suite *s = suite_create("requestor");
    TCase *tc_core = tcase_create("core");

    tcase_add_test(tc_core, test_requestor_init);
    tcase_add_test(tc_core, test_requestor_free);
    // tcase_add_test(tc_core, test_fetch_default_x509);
    // tcase_add_test(tc_core, test_fetch_all_x509);

    suite_add_tcase(s, tc_core);

    return s;
}
int main(int argc, char **argv)
{
    Suite *s = requestor_suite();
    SRunner *sr = srunner_create(s);
    testing::InitGoogleMock(&argc, argv);
    srunner_run_all(sr, CK_NORMAL);
    const int number_failed = srunner_ntests_failed(sr);
    
    srunner_free(sr);
    
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}