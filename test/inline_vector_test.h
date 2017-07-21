#ifndef INLINE_VECTOR_TEST_H_
#define INLINE_VECTOR_TEST_H_

#include "gtest/gtest.h"

#include "backend.h"
#include "inline_vector.h"
#include "io_util.h"

#define ROOTFILE "bdmFile.root"

namespace bdm {
namespace inline_vector_test_internal {

// IO related code must be in a header file.
inline void RunIOTest() {
  InlineVector<int, 8> neighbor;
  for (int i = 0; i < 15; i++) {
    neighbor.push_back(i);
  }

  OneElementArray<InlineVector<int, 8>> aoi_scalar(neighbor);
  std::vector<InlineVector<int, 8>> aoi_vector;
  for (int i = 0; i < 4; i++) {
    aoi_vector.push_back(neighbor);
  }

  WritePersistentObject(ROOTFILE, "InlineVector", neighbor, "RECREATE");
  WritePersistentObject(ROOTFILE, "S_InlineVector", aoi_scalar, "UPDATE");
  WritePersistentObject(ROOTFILE, "V_InlineVector", aoi_vector, "UPDATE");

  InlineVector<int, 8>* neighbor_r = nullptr;

  OneElementArray<InlineVector<int, 8>>* aoi_scalar_r = nullptr;
  std::vector<InlineVector<int, 8>>* aoi_vector_r = nullptr;

  GetPersistentObject(ROOTFILE, "InlineVector", neighbor_r);
  GetPersistentObject(ROOTFILE, "S_InlineVector", aoi_scalar_r);
  GetPersistentObject(ROOTFILE, "V_InlineVector", aoi_vector_r);

  EXPECT_EQ(neighbor.size(), neighbor_r->size());

  if (!(neighbor == (*neighbor_r))) {
    FAIL();
  }

  if (!(aoi_scalar[0] == (*aoi_scalar_r)[0])) {
    FAIL();
  }

  for (size_t i = 0; i < aoi_vector.size(); i++) {
    if (!(aoi_vector[i] == (*aoi_vector_r)[i])) {
      FAIL();
    }
  }

  remove(ROOTFILE);
}

}  // namespace inline_vector_test_internal
}  // namespace bdm

#endif
