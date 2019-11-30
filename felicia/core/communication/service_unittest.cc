// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gtest/gtest.h"
#include "third_party/chromium/base/rand_util.h"
#include "third_party/chromium/base/threading/platform_thread.h"

#include "felicia/core/communication/service_client.h"
#include "felicia/core/communication/service_server.h"
#include "felicia/core/lib/test/async_checker.h"
#include "felicia/core/message/test/simple_service.grpc.pb.h"

namespace felicia {

namespace {

class ServiceChecker : public AsyncChecker {
 public:
  void set_expected(const AddResponse& expected) { expected_ = expected; }

  void CheckResponse(const AddResponse& response) {
    EXPECT_EQ(expected_.sum(), response.sum());
    CountDownTest();
  }

 private:
  AddResponse expected_;
};

class GrpcSimpleClient : public rpc::Client<grpc::SimpleService> {
 public:
  FEL_GRPC_CLIENT_METHOD_DECLARE(Add);
};

FEL_GRPC_CLIENT_METHOD_DEFINE(GrpcSimpleClient, Add)

class GrpcSimpleService : public rpc::Service<grpc::SimpleService> {
 public:
  using rpc::Service<grpc::SimpleService>::Service;

 private:
  void EnqueueRequests() override;

  FEL_GRPC_SERVICE_METHOD_DECLARE(GrpcSimpleService, Add);

  void Add(const AddRequest* request, AddResponse* response,
           StatusOnceCallback callback);
};

void GrpcSimpleService::EnqueueRequests() {
  FEL_ENQUEUE_REQUEST(GrpcSimpleService, Add, false);
}

FEL_GRPC_SERVICE_METHOD_DEFINE(GrpcSimpleService, this, Add, false)

void GrpcSimpleService::Add(const AddRequest* request, AddResponse* response,
                            StatusOnceCallback callback) {
  int a = request->a();
  int b = request->b();

  response->set_sum(a + b);
  std::move(callback).Run(Status::OK());
}

}  // namespace

class ServiceTest : public testing::Test {
 public:
  ServiceTest() : service_("service") {}

  void RequestRegisterServer() { server_.RequestRegisterForTesting(service_); }

  void RequestRegisterClient(OnServiceConnectCallback on_connect_callback) {
    client_.RequestRegisterForTesting(service_, on_connect_callback);
  }

  void NotifyClient() {
    client_.OnFindServiceServer(server_.server_.service_info());
  }

  void Call(const AddRequest* request, AddResponse* response,
            StatusOnceCallback callback) {
    client_->AddAsync(request, response, std::move(callback));
  }

  void Release() {
    server_.RequestUnregisterForTesting(service_);
    client_.RequestUnregisterForTesting(service_);
  }

 protected:
  void SetUp() override {
    MainThread::SetBackground();
    MainThread::GetInstance().RunBackground();
  }

  std::string service_;
  ServiceServer<GrpcSimpleService> server_;
  ServiceClient<GrpcSimpleClient> client_;
};

void OnRequestAdd(const AddResponse* response, ServiceChecker* checker,
                  Status s) {
  EXPECT_TRUE(s.ok());
  checker->CheckResponse(*response);
}

void OnConnect(ServiceTest* test, const AddRequest* request,
               AddResponse* response, ServiceChecker* checker,
               ServiceInfo::Status s) {
  EXPECT_TRUE(s == ServiceInfo::REGISTERED);
  test->Call(request, response,
             base::BindOnce(&OnRequestAdd, response, checker));
}

void SetupClientAndServer(ServiceTest* test, const AddRequest* request,
                          AddResponse* response, ServiceChecker* checker) {
  test->RequestRegisterServer();
  test->RequestRegisterClient(
      base::BindRepeating(&OnConnect, test, request, response, checker));
  test->NotifyClient();
}

void RequestAndResponseService(ServiceTest* test) {
  ServiceChecker checker;
  AddRequest request;
  request.set_a(1);
  request.set_b(2);
  AddResponse response;
  response.set_sum(3);
  checker.set_expected(response);
  response.Clear();
  checker.set_test_num(1);
  checker.set_on_test_done(
      base::BindOnce(&ServiceTest::Release, base::Unretained(test)));
  MainThread& main_thread = MainThread::GetInstance();
  main_thread.PostTask(FROM_HERE,
                       base::BindOnce(&SetupClientAndServer, test, &request,
                                      &response, &checker));

  // Wait for client / server to change expected state.
  base::PlatformThread::Sleep(base::TimeDelta::FromMilliseconds(500));
  checker.ExpectTestCompleted();
}

TEST_F(ServiceTest, RequestAndResponseService) {
  RequestAndResponseService(this);
}

}  // namespace felicia