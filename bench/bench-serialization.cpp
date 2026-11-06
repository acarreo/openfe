#include <benchmark/benchmark.h>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <vector>

#include "openfe.h"
#include "bpdpvs.h"

using namespace openfe;

typedef struct {
    OpenFESchemeID scheme;
    std::string scheme_name;
    size_t vector_len;
} BenchmarkInput;

struct SchemeKeySet {
    std::unique_ptr<OpenFEMPK> mpk;
    std::unique_ptr<OpenFEMSK> msk;
    std::unique_ptr<OpenFECiphertext> ct;
    std::unique_ptr<OpenFESecretKey> sk;
};

static void initRelic() {
    static bool initialized = false;
    if (!initialized) {
        if (core_init() != RLC_OK) {
            std::cerr << "Error: core_init() failed\n";
            std::exit(1);
        }
        if (pc_param_set_any() != RLC_OK) {
            std::cerr << "Error: pc_param_set_any() failed\n";
            std::exit(1);
        }
        initialized = true;
    }
}

static std::vector<long> generateRandomVector(size_t len, long maxVal = 10) {
    static std::mt19937 rng(42);
    std::uniform_int_distribution<long> dist(-maxVal, maxVal);
    std::vector<long> vec(len);
    for (size_t i = 0; i < len; ++i) {
        vec[i] = dist(rng);
    }
    return vec;
}

static SchemeKeySet createKeySet(OpenFESchemeID scheme) {
    SchemeKeySet keys;
    switch (scheme) {
        case OpenFE_SCHEME_ABDP15:
            keys.mpk = std::make_unique<ABDP15MPK>();
            keys.msk = std::make_unique<ABDP15MSK>();
            keys.ct = std::make_unique<ABDP15Ciphertext>();
            keys.sk = std::make_unique<ABDP15SecretKey>();
            break;
        case OpenFE_SCHEME_ALS16:
            keys.mpk = std::make_unique<ALS16MPK>();
            keys.msk = std::make_unique<ALS16MSK>();
            keys.ct = std::make_unique<ALS16Ciphertext>();
            keys.sk = std::make_unique<ALS16SecretKey>();
            break;
        case OpenFE_SCHEME_KLMMRW16:
            keys.mpk = std::make_unique<KLMMRW16MPK>();
            keys.msk = std::make_unique<KLMMRW16MSK>();
            keys.ct = std::make_unique<KLMMRW16Ciphertext>();
            keys.sk = std::make_unique<KLMMRW16SecretKey>();
            break;
    }
    return keys;
}

static OpenABEByteString serializeMPK(OpenFESchemeID scheme, const OpenFEMPK& gen_mpk) {
    OpenABEByteString bytes;
    if (scheme == OpenFE_SCHEME_ABDP15) {
        const auto& mpk = static_cast<const ABDP15MPK&>(gen_mpk);
        mpk.g.serialize(bytes);
        for (const auto& h : mpk.h) h.serialize(bytes);
    } else if (scheme == OpenFE_SCHEME_ALS16) {
        const auto& mpk = static_cast<const ALS16MPK&>(gen_mpk);
        mpk.g.serialize(bytes);
        mpk.h.serialize(bytes);
        for (const auto& h : mpk.h_i) h.serialize(bytes);
    } else if (scheme == OpenFE_SCHEME_KLMMRW16) {
        const auto& mpk = static_cast<const KLMMRW16MPK&>(gen_mpk);
        mpk.g2.serialize(bytes);
        for (const auto& row : mpk.Bstar) {
            for (const auto& val : row) val.serialize(bytes);
        }
    }
    return bytes;
}

static OpenABEByteString serializeCiphertext(OpenFESchemeID scheme, const OpenFECiphertext& gen_ct) {
    OpenABEByteString bytes;
    if (scheme == OpenFE_SCHEME_ABDP15) {
        const auto& ct = static_cast<const ABDP15Ciphertext&>(gen_ct);
        ct.ct0.serialize(bytes);
        for (const auto& c : ct.ct) c.serialize(bytes);
    } else if (scheme == OpenFE_SCHEME_ALS16) {
        const auto& ct = static_cast<const ALS16Ciphertext&>(gen_ct);
        ct.C.serialize(bytes);
        ct.D.serialize(bytes);
        for (const auto& e : ct.E) e.serialize(bytes);
    } else if (scheme == OpenFE_SCHEME_KLMMRW16) {
        const auto& ct = static_cast<const KLMMRW16Ciphertext&>(gen_ct);
        ct.C1.serialize(bytes);
        for (const auto& c : ct.C2) c.serialize(bytes);
    }
    return bytes;
}

static OpenABEByteString serializeSecretKey(OpenFESchemeID scheme, const OpenFESecretKey& gen_sk) {
    OpenABEByteString bytes;
    if (scheme == OpenFE_SCHEME_ABDP15) {
        const auto& sk = static_cast<const ABDP15SecretKey&>(gen_sk);
        sk.sk_y.serialize(bytes);
        for (const auto& y : sk.y) y.serialize(bytes);
    } else if (scheme == OpenFE_SCHEME_ALS16) {
        const auto& sk = static_cast<const ALS16SecretKey&>(gen_sk);
        sk.s_y.serialize(bytes);
        sk.t_y.serialize(bytes);
        for (const auto& y : sk.y) y.serialize(bytes);
    } else if (scheme == OpenFE_SCHEME_KLMMRW16) {
        const auto& sk = static_cast<const KLMMRW16SecretKey&>(gen_sk);
        sk.K1.serialize(bytes);
        for (const auto& k : sk.K2) k.serialize(bytes);
    }
    return bytes;
}

static void BM_Serialize_MPK(benchmark::State& state, BenchmarkInput input) {
    initRelic();
    auto context = OpenFE_createContext(input.scheme, input.vector_len);
    auto keys = createKeySet(input.scheme);
    context->setup(*keys.mpk, *keys.msk);

    for (auto _ : state) {
        OpenABEByteString bytes = serializeMPK(input.scheme, *keys.mpk);
        benchmark::DoNotOptimize(bytes);
    }
    state.SetItemsProcessed(state.iterations());
    state.counters["Vector_Dim"] = input.vector_len;
}

static void BM_Serialize_Ciphertext(benchmark::State& state, BenchmarkInput input) {
    initRelic();
    auto context = OpenFE_createContext(input.scheme, input.vector_len);
    auto keys = createKeySet(input.scheme);
    context->setup(*keys.mpk, *keys.msk);
    auto x = generateRandomVector(input.vector_len);
    context->encrypt(*keys.mpk, x, *keys.ct);

    for (auto _ : state) {
        OpenABEByteString bytes = serializeCiphertext(input.scheme, *keys.ct);
        benchmark::DoNotOptimize(bytes);
    }
    state.SetItemsProcessed(state.iterations());
    state.counters["Vector_Dim"] = input.vector_len;
}

static void BM_Serialize_SecretKey(benchmark::State& state, BenchmarkInput input) {
    initRelic();
    auto context = OpenFE_createContext(input.scheme, input.vector_len);
    auto keys = createKeySet(input.scheme);
    context->setup(*keys.mpk, *keys.msk);
    auto y = generateRandomVector(input.vector_len);
    context->keygen(*keys.msk, y, *keys.sk);

    for (auto _ : state) {
        OpenABEByteString bytes = serializeSecretKey(input.scheme, *keys.sk);
        benchmark::DoNotOptimize(bytes);
    }
    state.SetItemsProcessed(state.iterations());
    state.counters["Vector_Dim"] = input.vector_len;
}

int main(int argc, char** argv) {
    initRelic();

    std::vector<OpenFESchemeID> schemes = {
        OpenFE_SCHEME_ABDP15,
        OpenFE_SCHEME_ALS16,
        OpenFE_SCHEME_KLMMRW16
    };

    std::vector<std::string> scheme_names = {
        "ABDP15",
        "ALS16",
        "KLMMRW16"
    };

    std::vector<size_t> dimensions = {1, 4, 16, 64, 256};

    for (size_t s = 0; s < schemes.size(); ++s) {
        for (size_t dim : dimensions) {
            BenchmarkInput input = {
                .scheme = schemes[s],
                .scheme_name = scheme_names[s],
                .vector_len = dim
            };

            std::string prefix = scheme_names[s] + "/dim_" + std::to_string(dim);

            benchmark::RegisterBenchmark(("BM_Serialize_MPK/" + prefix).c_str(), BM_Serialize_MPK, input);
            benchmark::RegisterBenchmark(("BM_Serialize_Ciphertext/" + prefix).c_str(), BM_Serialize_Ciphertext, input);
            benchmark::RegisterBenchmark(("BM_Serialize_SecretKey/" + prefix).c_str(), BM_Serialize_SecretKey, input);
        }
    }

    ::benchmark::Initialize(&argc, argv);
    ::benchmark::RunSpecifiedBenchmarks();

    core_clean();
    return 0;
}
