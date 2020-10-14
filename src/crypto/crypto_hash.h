#ifndef SRC_CRYPTO_CRYPTO_HASH_H_
#define SRC_CRYPTO_CRYPTO_HASH_H_

#if defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS

#include "crypto/crypto_keys.h"
#include "crypto/crypto_util.h"
#include "allocated_buffer.h"
#include "base_object.h"
#include "env.h"
#include "memory_tracker.h"
#include "v8.h"

namespace node {
namespace crypto {
class Hash final : public BaseObject {
 public:
  ~Hash() override;

  static void Initialize(Environment* env, v8::Local<v8::Object> target);

  void MemoryInfo(MemoryTracker* tracker) const override;
  SET_MEMORY_INFO_NAME(Hash)
  SET_SELF_SIZE(Hash)

  bool HashInit(const EVP_MD* md, v8::Maybe<unsigned int> xof_md_len);
  bool HashUpdate(const char* data, size_t len);

  static void GetHashes(const v8::FunctionCallbackInfo<v8::Value>& args);

 protected:
  static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void HashUpdate(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void HashDigest(const v8::FunctionCallbackInfo<v8::Value>& args);

  Hash(Environment* env, v8::Local<v8::Object> wrap);

 private:
  EVPMDPointer mdctx_;
  bool has_md_;
  unsigned int md_len_;
  unsigned char* md_value_;
};

struct HashConfig final : public MemoryRetainer {
  CryptoJobMode mode;
  ByteSource in;
  const EVP_MD* digest;
  unsigned int length;

  HashConfig() = default;

  explicit HashConfig(HashConfig&& other) noexcept;

  HashConfig& operator=(HashConfig&& other) noexcept;

  void MemoryInfo(MemoryTracker* tracker) const override;
  SET_MEMORY_INFO_NAME(HashConfig);
  SET_SELF_SIZE(HashConfig);
};

struct HashTraits final {
  using AdditionalParameters = HashConfig;
  static constexpr const char* JobName = "HashJob";
  static constexpr AsyncWrap::ProviderType Provider =
      AsyncWrap::PROVIDER_HASHREQUEST;

  static v8::Maybe<bool> AdditionalConfig(
      CryptoJobMode mode,
      const v8::FunctionCallbackInfo<v8::Value>& args,
      unsigned int offset,
      HashConfig* params);

  static bool DeriveBits(
      Environment* env,
      const HashConfig& params,
      ByteSource* out);

  static v8::Maybe<bool> EncodeOutput(
      Environment* env,
      const HashConfig& params,
      ByteSource* out,
      v8::Local<v8::Value>* result);
};

using HashJob = DeriveBitsJob<HashTraits>;

}  // namespace crypto
}  // namespace node

#endif  // defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS
#endif  // SRC_CRYPTO_CRYPTO_HASH_H_