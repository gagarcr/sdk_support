#include "em_device.h"

#if defined(CRYPTOACC_PRESENT)

#include "sli_cryptoacc_transparent_types.h"
#include "sli_cryptoacc_transparent_functions.h"
#include "cryptoacc_management.h"
// Replace inclusion of psa/crypto_xxx.h with the new psa driver commong
// interface header file when it becomes available.
#include "psa/crypto_platform.h"
#include "psa/crypto_sizes.h"
#include "psa/crypto_struct.h"
#include "sx_dh_alg.h"
#include "sx_ecc_curves.h"
#include "sx_primitives.h"
#include "sx_errors.h"
#include "cryptolib_types.h"

#include <string.h>

psa_status_t sli_cryptoacc_transparent_key_agreement(psa_algorithm_t alg,
                                                     const psa_key_attributes_t *attributes,
                                                     const uint8_t *key_buffer,
                                                     size_t key_buffer_size,
                                                     const uint8_t *peer_key,
                                                     size_t peer_key_length,
                                                     uint8_t *output,
                                                     size_t output_size,
                                                     size_t *output_length)
{
  psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
  uint32_t sx_ret = CRYPTOLIB_CRYPTO_ERR;
  uint32_t curve_flags = 0;
  block_t domain = NULL_blk;
  uint8_t tmp_output_buf[64] = { 0 };

  // Argument check.
  if (key_buffer == NULL
      || peer_key == NULL
      || output == NULL
      || output_length == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  psa_key_type_t key_type = psa_get_key_type(attributes);
  size_t key_bits = psa_get_key_bits(attributes);

  // Check that key_buffer contains private key.
  if (PSA_KEY_TYPE_IS_PUBLIC_KEY(key_type)) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  // Only accelerate ECDH over secp{192, 224, 256}r1 curves.
  if (!PSA_ALG_IS_ECDH(alg)
      || key_type != PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_CURVE_SECP_R1)) {
    return PSA_ERROR_NOT_SUPPORTED;
  }

  switch (key_bits) {
    case 192:
      curve_flags = sx_ecc_curve_p192.pk_flags;
      domain = block_t_convert(sx_ecc_curve_p192.params.addr,
                               6 * sx_ecc_curve_p192.bytesize);
      break;
    case 224:
      curve_flags = sx_ecc_curve_p224.pk_flags;
      domain = block_t_convert(sx_ecc_curve_p224.params.addr,
                               6 * sx_ecc_curve_p224.bytesize);
      break;
    case 256:
      curve_flags = sx_ecc_curve_p256.pk_flags;
      domain = block_t_convert(sx_ecc_curve_p256.params.addr,
                               6 * sx_ecc_curve_p256.bytesize);
      break;
    default:
      return PSA_ERROR_NOT_SUPPORTED;
  }

  // Check input-keys sizes.
  if (key_buffer_size < PSA_BITS_TO_BYTES(key_bits)
      || peer_key_length < PSA_BITS_TO_BYTES(key_bits) * 2 + 1) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  // Check sufficient output buffer size.
  if (output_size < PSA_BITS_TO_BYTES(key_bits)) {
    return PSA_ERROR_BUFFER_TOO_SMALL;
  }

  block_t priv = block_t_convert(key_buffer, PSA_BITS_TO_BYTES(key_bits));
  block_t pub = block_t_convert(peer_key + 1, PSA_BITS_TO_BYTES(key_bits) * 2);
  block_t shared_key = block_t_convert(tmp_output_buf, PSA_BITS_TO_BYTES(key_bits) * 2);

  // Check peer_key is a public key of correct format.
  if (peer_key[0] != 0x04) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  // Check that public key is a point on the curve.
  status = cryptoacc_management_acquire();
  if (status != PSA_SUCCESS) {
    return status;
  }
  sx_ret = ecc_is_point_on_curve(domain,
                                 pub,
                                 PSA_BITS_TO_BYTES(key_bits),
                                 curve_flags);
  if (sx_ret != CRYPTOLIB_SUCCESS) {
    cryptoacc_management_release();
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  // Compute shared key.
  sx_ret = dh_shared_key_ecdh(domain, priv, pub, shared_key, PSA_BITS_TO_BYTES(key_bits), curve_flags);
  status = cryptoacc_management_release();
  if (sx_ret != CRYPTOLIB_SUCCESS
      || status != PSA_SUCCESS) {
    return PSA_ERROR_HARDWARE_FAILURE;
  }

  memcpy(output, tmp_output_buf, PSA_BITS_TO_BYTES(key_bits));
  memset(tmp_output_buf, 0, sizeof(tmp_output_buf));
  *output_length = PSA_BITS_TO_BYTES(key_bits);

  return PSA_SUCCESS;
}

#endif // defined(CRYPTOACC_PRESENT)
