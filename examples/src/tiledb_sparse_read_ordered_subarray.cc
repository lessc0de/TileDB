/**
 * @file   tiledb_sparse_read_ordered_subarray.cc
 *
 * @section LICENSE
 *
 * The MIT License
 *
 * @copyright Copyright (c) 2017 TileDB, Inc.
 * @copyright Copyright (c) 2016 MIT and Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * @section DESCRIPTION
 *
 * It shows how to read from a sparse array, constraining the read
 * to a specific subarray and subset of attributes. This time the cells are
 * returned in row-major order within the specified subarray.
 *
 * You need to run the following to make it work:
 *
 * $ ./tiledb_sparse_create
 * $ ./tiledb_sparse_write_global_1
 * $ ./tiledb_sparse_read_ordered_subarray
 */

#include <tiledb.h>
#include <cstdio>

int main() {
  // Create TileDB context
  tiledb_ctx_t* ctx;
  tiledb_ctx_create(&ctx);

  // Prepare cell buffers
  int buffer_a1[10];
  uint64_t buffer_a2[10];
  char buffer_var_a2[30];
  float buffer_a3[20];
  uint64_t buffer_coords[20];
  void* buffers[] = {
      buffer_a1, buffer_a2, buffer_var_a2, buffer_a3, buffer_coords};
  uint64_t buffer_sizes[] = {sizeof(buffer_a1),
                             sizeof(buffer_a2),
                             sizeof(buffer_var_a2),
                             sizeof(buffer_a3),
                             sizeof(buffer_coords)};

  // Create query
  uint64_t subarray[] = {3, 4, 2, 4};
  tiledb_query_t* query;
  tiledb_query_create(
      ctx,
      &query,
      "my_sparse_array",
      TILEDB_READ,
      TILEDB_ROW_MAJOR,
      subarray,
      nullptr,
      0,
      buffers,
      buffer_sizes);

  // Submit query
  tiledb_query_submit(ctx, query);

  // Print cell values
  uint64_t result_num = buffer_sizes[0] / sizeof(int);
  printf("result num: %llu\n\n", result_num);
  printf("coords\t  a1\t   a2\t      (a3.first, a3.second)\n");
  printf("---------------------------------------------------\n");
  for (uint64_t i = 0; i < result_num; ++i) {
    printf("(%lld, %lld)", buffer_coords[2 * i], buffer_coords[2 * i + 1]);
    printf("\t %3d", buffer_a1[i]);
    size_t var_size = (i != result_num - 1) ? buffer_a2[i + 1] - buffer_a2[i] :
                                              buffer_sizes[2] - buffer_a2[i];
    printf("\t %4.*s", int(var_size), &buffer_var_a2[buffer_a2[i]]);
    printf("\t\t (%5.1f, %5.1f)\n", buffer_a3[2 * i], buffer_a3[2 * i + 1]);
  }

  // Clean up
  tiledb_query_free(ctx, query);
  tiledb_ctx_free(ctx);

  return 0;
}
