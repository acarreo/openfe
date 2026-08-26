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

static void BM_Setup(benchmark::State& state, BenchmarkInput input) {
  initRelic();
  for (auto _ : state) {
    auto context = OpenFE_createContext(input.scheme, input.vector_len);
    auto keys = createKeySet(input.scheme);
    context->setup(*keys.mpk, *keys.msk);
    benchmark::DoNotOptimize(keys);
  }
  state.SetItemsProcessed(state.iterations());
  state.counters["Vector_Dim"] = input.vector_len;
}

static void BM_KeyGen(benchmark::State& state, BenchmarkInput input) {
  initRelic();
  auto context = OpenFE_createContext(input.scheme, input.vector_len);
  auto keys = createKeySet(input.scheme);
  context->setup(*keys.mpk, *keys.msk);
  auto y = generateRandomVector(input.vector_len);

  for (auto _ : state) {
    context->keygen(*keys.msk, y, *keys.sk);
    benchmark::DoNotOptimize(keys.sk);
  }
  state.SetItemsProcessed(state.iterations());
  state.counters["Vector_Dim"] = input.vector_len;
}

static void BM_Encrypt(benchmark::State& state, BenchmarkInput input) {
  initRelic();
  auto context = OpenFE_createContext(input.scheme, input.vector_len);
  auto keys = createKeySet(input.scheme);
  context->setup(*keys.mpk, *keys.msk);
  auto x = generateRandomVector(input.vector_len);

  for (auto _ : state) {
    context->encrypt(*keys.mpk, x, *keys.ct);
    benchmark::DoNotOptimize(keys.ct);
  }
  state.SetItemsProcessed(state.iterations());
  state.counters["Vector_Dim"] = input.vector_len;
}

static void BM_Decrypt(benchmark::State& state, BenchmarkInput input) {
  initRelic();
  auto context = OpenFE_createContext(input.scheme, input.vector_len);
  auto keys = createKeySet(input.scheme);
  context->setup(*keys.mpk, *keys.msk);
  auto x = generateRandomVector(input.vector_len);
  auto y = generateRandomVector(input.vector_len);

  context->encrypt(*keys.mpk, x, *keys.ct);
  context->keygen(*keys.msk, y, *keys.sk);

  for (auto _ : state) {
    long res = context->decrypt(*keys.mpk, *keys.sk, *keys.ct);
    benchmark::DoNotOptimize(res);
  }
  state.SetItemsProcessed(state.iterations());
  state.counters["Vector_Dim"] = input.vector_len;
}

static void BM_CompleteCycle(benchmark::State& state, BenchmarkInput input) {
  initRelic();
  auto x = generateRandomVector(input.vector_len);
  auto y = generateRandomVector(input.vector_len);

  for (auto _ : state) {
    auto context = OpenFE_createContext(input.scheme, input.vector_len);
    auto keys = createKeySet(input.scheme);
    context->setup(*keys.mpk, *keys.msk);
    context->encrypt(*keys.mpk, x, *keys.ct);
    context->keygen(*keys.msk, y, *keys.sk);
    long res = context->decrypt(*keys.mpk, *keys.sk, *keys.ct);
    benchmark::DoNotOptimize(res);
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

      benchmark::RegisterBenchmark(("BM_Setup/" + prefix).c_str(), BM_Setup, input);
      benchmark::RegisterBenchmark(("BM_KeyGen/" + prefix).c_str(), BM_KeyGen, input);
      benchmark::RegisterBenchmark(("BM_Encrypt/" + prefix).c_str(), BM_Encrypt, input);
      benchmark::RegisterBenchmark(("BM_Decrypt/" + prefix).c_str(), BM_Decrypt, input);
      benchmark::RegisterBenchmark(("BM_CompleteCycle/" + prefix).c_str(), BM_CompleteCycle, input);
    }
  }

  ::benchmark::Initialize(&argc, argv);
  ::benchmark::RunSpecifiedBenchmarks();

  core_clean();
  return 0;
}
