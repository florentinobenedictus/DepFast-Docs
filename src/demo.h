#pragma once

#include "rrr.hpp"

#include <errno.h>


namespace demo {

class DemoService: public rrr::Service {
public:
    enum {
        HELLO = 0x647cd2e4,
        SUM = 0x14d5d452,
    };
    int __reg_to__(rrr::Server* svr) {
        int ret = 0;
        if ((ret = svr->reg(HELLO, this, &DemoService::__hello__wrapper__)) != 0) {
            goto err;
        }
        if ((ret = svr->reg(SUM, this, &DemoService::__sum__wrapper__)) != 0) {
            goto err;
        }
        return 0;
    err:
        svr->unreg(HELLO);
        svr->unreg(SUM);
        return ret;
    }
    // these RPC handler functions need to be implemented by user
    // for 'raw' handlers, remember to reply req, delete req, and sconn->release(); use sconn->run_async for heavy job
    virtual void hello(const std::string& hi, std::string* reply, rrr::DeferredReply* defer) = 0;
    virtual void sum(const rrr::i32& a, const rrr::i32& b, const rrr::i32& c, rrr::i32* result, rrr::DeferredReply* defer) = 0;
private:
    void __hello__wrapper__(rrr::Request* req, rrr::ServerConnection* sconn) {
        std::string* in_0 = new std::string;
        req->m >> *in_0;
        std::string* out_0 = new std::string;
        Log_debug("INSIDE HELLO WRAPPER");
        auto __marshal_reply__ = [=] {
            *sconn << *out_0;
        };
        auto __cleanup__ = [=] {
            delete in_0;
            delete out_0;
        };
        rrr::DeferredReply* __defer__ = new rrr::DeferredReply(req, sconn, __marshal_reply__, __cleanup__);
        this->hello(*in_0, out_0, __defer__);
    }
    void __sum__wrapper__(rrr::Request* req, rrr::ServerConnection* sconn) {
        rrr::i32* in_0 = new rrr::i32;
        req->m >> *in_0;
        rrr::i32* in_1 = new rrr::i32;
        req->m >> *in_1;
        rrr::i32* in_2 = new rrr::i32;
        req->m >> *in_2;
        rrr::i32* out_0 = new rrr::i32;
        auto __marshal_reply__ = [=] {
            *sconn << *out_0;
        };
        auto __cleanup__ = [=] {
            delete in_0;
            delete in_1;
            delete in_2;
            delete out_0;
        };
        rrr::DeferredReply* __defer__ = new rrr::DeferredReply(req, sconn, __marshal_reply__, __cleanup__);
        this->sum(*in_0, *in_1, *in_2, out_0, __defer__);
    }
};

class DemoProxy {
protected:
    rrr::Client* __cl__;
public:
    DemoProxy(rrr::Client* cl): __cl__(cl) { }
    rrr::Future* async_hello(const std::string& hi, const rrr::FutureAttr& __fu_attr__ = rrr::FutureAttr()) {
        rrr::Future* __fu__ = __cl__->begin_request(DemoService::HELLO, __fu_attr__);
        if (__fu__ != nullptr) {
            *__cl__ << hi;
        }
        __cl__->end_request();
        return __fu__;
    }
    rrr::i32 hello(const std::string& hi, std::string* reply) {
        rrr::Future* __fu__ = this->async_hello(hi);
        if (__fu__ == nullptr) {
            return ENOTCONN;
        }
        rrr::i32 __ret__ = __fu__->get_error_code();
        if (__ret__ == 0) {
            __fu__->get_reply() >> *reply;
        }
        __fu__->release();
        return __ret__;
    }
    rrr::Future* async_sum(const rrr::i32& a, const rrr::i32& b, const rrr::i32& c, const rrr::FutureAttr& __fu_attr__ = rrr::FutureAttr()) {
        rrr::Future* __fu__ = __cl__->begin_request(DemoService::SUM, __fu_attr__);
        if (__fu__ != nullptr) {
            *__cl__ << a;
            *__cl__ << b;
            *__cl__ << c;
        }
        __cl__->end_request();
        return __fu__;
    }
    rrr::i32 sum(const rrr::i32& a, const rrr::i32& b, const rrr::i32& c, rrr::i32* result) {
        rrr::Future* __fu__ = this->async_sum(a, b, c);
        if (__fu__ == nullptr) { // Case 1: Server failure before rpc
            return ENOTCONN;
        }
        rrr::i32 __ret__ = __fu__->get_error_code();
        if (__ret__ == 0) {
            __fu__->get_reply() >> *result;
        }
        __fu__->release();
        return __ret__; // Case 2: Server failure during rpc
    }
};

} // namespace demo



