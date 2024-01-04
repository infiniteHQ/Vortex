#include <catch2/catch_all.hpp>
#include <string>

#include "../../htk.h"
#include "../../htk_internals.h"

#define CATCH_CONFIG_MAIN

#define RECEIVER_ADDED_TO_CTX false
#define PROVIDER_STARTED false


struct SampleMatrix : public HToolkitMatrix{
};
ADD_MATRIX_TO_FACTORY(Sample, SampleMatrix);


struct SampleProvider : public HToolkitProvider{
    virtual void Start() override {
        #undef PROVIDER_STARTED
        #define PROVIDER_STARTED true
    }
};
ADD_PROVIDER_TO_FACTORY(HSTD, SampleProvider);

// End-User API

TEST_CASE("CreateContext", "[context]"){
    HtkContext* ctx = HToolkit::CreateContext();
    REQUIRE(ctx != nullptr);
    delete(ctx);
}
