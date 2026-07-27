//
//  vortex.cpp
//  Sources of main context features
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "../include/vortex.h"
#ifndef VORTEX_DISABLE
#include "../include/vortex_internals.h"

// runtime pointer
#ifndef CVortexMaker
std::weak_ptr<VxContext> CVortexMaker;
#endif

std::shared_ptr<VxContext> vxe::create_context() {
  auto prev_ctx = get_current_context();
  auto ctx = std::make_shared<VxContext>();

  set_current_context(ctx);
  initialize();

  if (prev_ctx != nullptr)
    set_current_context(prev_ctx);

  return ctx;
}

void vxe::set_current_context(std::shared_ptr<VxContext> ctx) {
  CVortexMaker = ctx;
}

std::shared_ptr<VxContext> vxe::get_current_context() {
  return CVortexMaker.lock();
}

void vxe::destroy_context(std::shared_ptr<VxContext> ctx) {
  auto prev_ctx = get_current_context();
  if (ctx == nullptr)
    ctx = prev_ctx;

  set_current_context((prev_ctx != ctx) ? prev_ctx : nullptr);
}

void vxe::initialize() {
  auto ctx = get_current_context();
  assert(ctx != nullptr && "initialize() called without context !");
  ctx->initialized = true;
}

VortexNet::VortexNet() {
  naettInit(nullptr);
}

VortexNet::~VortexNet() {
}

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#endif

bool VortexNet::CheckNet() {
#ifdef _WIN32
  int rc = std::system("ping -n 1 -w 2000 1.1.1.1 >NUL 2>&1");
  if (rc != 0)
    rc = std::system("ping -n 1 -w 2000 8.8.8.8 >NUL 2>&1");
  return rc == 0;
#else
  addrinfo hints{}, *res = nullptr;
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  int status = getaddrinfo("infinite.si", "80", &hints, &res);

  if (res)
    freeaddrinfo(res);

  return (status == 0);
#endif
}

std::string VortexNet::GET(const std::string &url) {
  return Request(url, "GET");
}

std::string VortexNet::POST(const std::string &url, const std::string &body, const std::string &contentType) {
  return Request(url, "POST", body, contentType);
}

std::string VortexNet::Request(
    const std::string &url,
    const std::string &method,
    const std::string &body,
    const std::string &contentType) {
  naettReq *req = nullptr;

  if (method == "GET") {
    std::cout << "GET request\n";
    req = naettRequest_va(url.c_str(), 2, naettMethod("GET"), naettHeader("accept", "*/*"));
  } else if (method == "POST") {
    std::cout << "POST request\n";
    req = naettRequest_va(
        url.c_str(),
        4,
        naettMethod("POST"),
        naettHeader("Content-Type", contentType.c_str()),
        naettBody(body.c_str(), (int)body.size()),
        naettHeader("accept", "*/*"));
  } else {
    std::cerr << "Unsupported HTTP method: " << method << std::endl;
    return "";
  }

  if (!req) {
    std::cerr << "Failed to create request\n";
    return "";
  }

  naettRes *res = naettMake(req);
  if (!res) {
    std::cerr << "Failed to make request\n";
    naettFree(req);
    return "";
  }

  while (!naettComplete(res)) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  int status = naettGetStatus(res);
  if (status < 0) {
    std::cerr << "Request failed with status: " << status << std::endl;
    naettClose(res);
    naettFree(req);
    return "";
  }

  int length = 0;
  const char *responseBody = static_cast<const char *>(naettGetBody(res, &length));
  std::string result;

  if (responseBody && length > 0) {
    result.assign(responseBody, length);
  } else {
    std::cerr << "Empty response body\n";
    result = "";
  }

  naettClose(res);
  naettFree(req);

  return result;
}

#endif  // VORTEX_DISABLE