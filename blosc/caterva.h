/*********************************************************************
  Blosc - Blocked Shuffling and Compression Library

  Copyright (C) 2021  The Blosc Developers <blosc@blosc.org>
  https://blosc.org
  License: BSD 3-Clause (see LICENSE.txt)

  See LICENSE.txt for details about copyright and rights to use.
**********************************************************************/

/** @file caterva.h
 * @brief Caterva header file.
 *
 * This file contains Caterva public API and the structures needed to use it.
 * @author Blosc Development team <blosc@blosc.org>
 */

#ifndef CATERVA_CATERVA_H_
#define CATERVA_CATERVA_H_

#include <blosc2.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif


/* The version for metalayer format; starts from 0 and it must not exceed 127 */
#define CATERVA_METALAYER_VERSION 0

/* The maximum number of dimensions for caterva arrays */
#define CATERVA_MAX_DIM 8

/* The maximum number of metalayers for caterva arrays */
#define CATERVA_MAX_METALAYERS (BLOSC2_MAX_METALAYERS - 1)

/**
 * @brief General parameters needed for the creation of a caterva array.
 */
typedef struct {
  int8_t ndim;
  //!< The array dimensions.
  int64_t shape[CATERVA_MAX_DIM];
  //!< The array shape.
  int32_t chunkshape[CATERVA_MAX_DIM];
  //!< The shape of each chunk of Blosc.
  int32_t blockshape[CATERVA_MAX_DIM];
  //!< The shape of each block of Blosc.
  blosc2_storage *b2_storage;
  //!< The Blosc storage properties
  blosc2_metalayer metalayers[CATERVA_MAX_METALAYERS];
  //!< List with the metalayers desired.
  int32_t nmetalayers;
  //!< The number of metalayers.
} caterva_context_t;

/**
 * @brief An *optional* cache for a single block.
 *
 * When a chunk is needed, it is copied into this cache. In this way, if the same chunk is needed
 * again afterwards, it is not necessary to recover it because it is already in the cache.
 */
struct chunk_cache_s {
  uint8_t *data;
  //!< The chunk data.
  int64_t nchunk;
  //!< The chunk number in cache. If @p nchunk equals to -1, it means that the cache is empty.
};

/**
 * @brief A multidimensional array of data that can be compressed.
 */
typedef struct {
  blosc2_schunk *sc;
  //!< Pointer to a Blosc super-chunk
  int64_t shape[CATERVA_MAX_DIM];
  //!< Shape of original data.
  int32_t chunkshape[CATERVA_MAX_DIM];
  //!< Shape of each chunk.
  int64_t extshape[CATERVA_MAX_DIM];
  //!< Shape of padded data.
  int32_t blockshape[CATERVA_MAX_DIM];
  //!< Shape of each block.
  int64_t extchunkshape[CATERVA_MAX_DIM];
  //!< Shape of padded chunk.
  int64_t nitems;
  //!< Number of items in original data.
  int32_t chunknitems;
  //!< Number of items in each chunk.
  int64_t extnitems;
  //!< Number of items in padded data.
  int32_t blocknitems;
  //!< Number of items in each block.
  int64_t extchunknitems;
  //!< Number of items in a padded chunk.
  int8_t ndim;
  //!< Data dimensions.
  struct chunk_cache_s chunk_cache;
  //!< A partition cache.
  int64_t item_array_strides[CATERVA_MAX_DIM];
  //!< Item - shape strides.
  int64_t item_chunk_strides[CATERVA_MAX_DIM];
  //!< Item - shape strides.
  int64_t item_extchunk_strides[CATERVA_MAX_DIM];
  //!< Item - shape strides.
  int64_t item_block_strides[CATERVA_MAX_DIM];
  //!< Item - shape strides.
  int64_t block_chunk_strides[CATERVA_MAX_DIM];
  //!< Item - shape strides.
  int64_t chunk_array_strides[CATERVA_MAX_DIM];
  //!< Item - shape strides.
} caterva_array_t;


/**
 * @brief Create caterva params.
 *
 * @param b2_storage The Blosc2 storage params.
 * @param ndim The dimensions.
 * @param shape The shape.
 * @param chunkshape The chunk shape.
 * @param blockshape The block shape.
 * @param metalayers The memory pointer to the list of the metalayers desired.
 * @param nmetalayers The number of metalayers.
 *
 * @return A pointer to the new caterva params. NULL is returned if this fails.
 *
 * @note The pointer returned must be freed when not used anymore with #caterva_free_ctx.
 *
 */
caterva_context_t *caterva_create_ctx(blosc2_storage *b2_storage, int8_t ndim, int64_t *shape,
                                      int32_t *chunkshape, int32_t *blockshape,
                                      blosc2_metalayer *metalayers, int32_t nmetalayers);


/**
 * @brief Free the resources associated with caterva_context_t.
 *
 * @param ctx The caterva context to free.
 *
 * @return An error code.
 *
 * @note This is safe in the sense that it will not free the schunk pointer in internal cparams.
 *
 */
int caterva_free_ctx(caterva_context_t *ctx);


/**
 * @brief Create an uninitialized array.
 *
 * @param ctx The caterva context for the new array.
 * @param array The memory pointer where the array will be created.
 *
 * @return An error code.
 */
int caterva_uninit(caterva_context_t *ctx, caterva_array_t **array);


/**
 * @brief Create an empty array.
 *
 * @param ctx The caterva context for the new array.
 * @param array The memory pointer where the array will be created.
 *
 * @return An error code.
 */
int caterva_empty(caterva_context_t *ctx, caterva_array_t **array);


/**
 * Create an array, with zero being used as the default value for
 * uninitialized portions of the array.
 *
 * @param ctx The caterva context for the new array.
 * @param array The memory pointer where the array will be created.
 *
 * @return An error code.
 */
int caterva_zeros(caterva_context_t *ctx, caterva_array_t **array);


/**
 * Create an array, with @p fill_value being used as the default value for
 * uninitialized portions of the array.
 *
 * @param ctx The caterva context for the new array.
 * @param fill_value Default value for uninitialized portions of the array.
 * @param array The memory pointer where the array will be created.
 *
 * @return An error code.
 */
int caterva_full(caterva_context_t *ctx, caterva_array_t **array, void *fill_value);

/**
 * @brief Free an array.
 *
 * @param array The array.
 *
 * @return An error code.
 */
int caterva_free(caterva_array_t *array);

/**
 * @brief Create a caterva array from a super-chunk. It can only be used if the array
 * is backed by a blosc super-chunk.
 *
 * @param schunk The blosc super-chunk where the caterva array is stored.
 * @param array The memory pointer where the array will be created.
 *
 * @return An error code.
 */
int caterva_from_schunk(blosc2_schunk *schunk, caterva_array_t **array);

/**
 * Create a serialized super-chunk from a caterva array.
 *
 * @param array The caterva array to be serialized.
 * @param cframe The pointer of the buffer where the in-memory array will be copied.
 * @param cframe_len The length of the in-memory array buffer.
 * @param needs_free Whether the buffer should be freed or not.
 *
 * @return An error code
 */
int caterva_to_cframe(caterva_array_t *array, uint8_t **cframe,
                      int64_t *cframe_len, bool *needs_free);

/**
 * @brief Create a caterva array from a serialized super-chunk.
 *
 * @param cframe The buffer of the in-memory array.
 * @param cframe_len The size (in bytes) of the in-memory array.
 * @param copy Whether caterva should make a copy of the cframe data or not. The copy will be made to an internal sparse frame.
 * @param array The memory pointer where the array will be created.
 *
 * @return An error code.
 */
int caterva_from_cframe(uint8_t *cframe, int64_t cframe_len, bool copy, caterva_array_t **array);

/**
 * @brief Read a caterva array from disk.
 *
 * @param urlpath The urlpath of the caterva array on disk.
 * @param array The memory pointer where the array will be created.
 *
 * @return An error code.
 */
int caterva_open(const char *urlpath, caterva_array_t **array);

/**
 * @brief Save caterva array into a specific urlpath.
 *
 * @param array The array to be saved.
 * @param urlpath The urlpath where the array will be stored.
 *
 * @return An error code.
 */
BLOSC_EXPORT int caterva_save(caterva_array_t *array, char *urlpath);

/**
 * @brief Create a caterva array from the data stored in a buffer.
 *
 * @param ctx The caterva context for the new array.
 * @param array The memory pointer where the array will be created.
 * @param buffer The buffer where source data is stored.
 * @param buffersize The size (in bytes) of the buffer.
 *
 * @return An error code.
 */
BLOSC_EXPORT int caterva_from_buffer(caterva_context_t *ctx, caterva_array_t **array, void *buffer, int64_t buffersize);

/**
 * @brief Extract the data into a C buffer from a caterva array.
 *
 * @param array The caterva array.
 * @param buffer The buffer where the data will be stored.
 * @param buffersize Size (in bytes) of the buffer.
 *
 * @return An error code.
 */
BLOSC_EXPORT int caterva_to_buffer(caterva_array_t *array, void *buffer,
                                   int64_t buffersize);

/**
 * @brief Get a slice from an array and store it into a new array.
 *
 * @param ctx The caterva context for the new array.
 * @param array The memory pointer where the array will be created.
 * @param src The array from which the slice will be extracted
 * @param start The coordinates where the slice will begin.
 * @param stop The coordinates where the slice will end.
 *
 * @return An error code.
 *
 * @note The ndim and shape from ctx will be overwritten by the src and stop-start respectively.
 *
 */
int caterva_get_slice(caterva_context_t *ctx, caterva_array_t **array, caterva_array_t *src, const int64_t *start,
                      const int64_t *stop);

/**
 * @brief Squeeze a caterva array
 *
 * This function remove selected single-dimensional entries from the shape of a
 caterva array.
 *
 * @param array The caterva array.
 * @param index Indexes of the single-dimensional entries to remove.
 *
 * @return An error code
 */
int caterva_squeeze_index(caterva_array_t *array, const bool *index);

/**
 * @brief Squeeze a caterva array
 *
 * This function remove single-dimensional entries from the shape of a caterva array.
 *
 * @param array The caterva array.
 *
 * @return An error code
 */
int caterva_squeeze(caterva_array_t *array);

/**
 * @brief Get a slice from an array and store it into a C buffer.
 *
 * @param array The array from which the slice will be extracted.
 * @param start The coordinates where the slice will begin.
 * @param stop The coordinates where the slice will end.
 * @param buffershape The shape of the buffer.
 * @param buffer The buffer where the data will be stored.
 * @param buffersize The size (in bytes) of the buffer.
 *
 * @return An error code.
 */
int caterva_get_slice_buffer(caterva_array_t *array,
                             int64_t *start, int64_t *stop,
                             void *buffer, int64_t *buffershape, int64_t buffersize);

/**
 * @brief Set a slice into a caterva array from a C buffer.
 *
 * @param buffer The buffer where the slice data is.
 * @param buffersize The size (in bytes) of the buffer.
 * @param start The coordinates where the slice will begin.
 * @param stop The coordinates where the slice will end.
 * @param buffershape The shape of the buffer.
 * @param array The caterva array where the slice will be set
 *
 * @return An error code.
 */
int caterva_set_slice_buffer(void *buffer, int64_t *buffershape, int64_t buffersize,
                             int64_t *start, int64_t *stop, caterva_array_t *array);

/**
 * @brief Make a copy of the array data. The copy is done into a new caterva array.
 *
 * @param ctx The caterva context for the new array.
 * @param src The array from which data is copied.
 * @param array The memory pointer where the array will be created.
 *
 * @return An error code
 *
 * @note The ndim and shape in ctx will be overwritten by the src ctx.
 *
 */
int caterva_copy(caterva_context_t *ctx, caterva_array_t *src, caterva_array_t **array);

/**
 * @brief Print metalayer parameters.
 *
 * @param array The array where the metalayer is stored.
 *
 * @return An error code
 */
int caterva_print_meta(caterva_array_t *array);

/**
 * @brief Resize the shape of an array
 *
 * @param array The array to be resized.
 * @param new_shape The new shape from the array.
 * @param start The position in which the array will be extended or shrinked.
 *
 * @return An error code
 */
int caterva_resize(caterva_array_t *array, const int64_t *new_shape, const int64_t *start);


/**
 * @brief Insert given buffer in an array extending the given axis.
 *
 * @param array The array to insert the data.
 * @param buffer The buffer data to be inserted.
 * @param buffersize The size (in bytes) of the buffer.
 * @param axis The axis that will be extended.
 * @param insert_start The position inside the axis to start inserting the data.
 *
 * @return An error code.
 */
int caterva_insert(caterva_array_t *array, void *buffer, int64_t buffersize,
                   const int8_t axis, int64_t insert_start);

/**
 * Append a buffer at the end of a caterva array.
 *
 * @param array The caterva array.
 * @param buffer The buffer where the data is stored.
 * @param buffersize Size (in bytes) of the buffer.
 * @param axis The axis that will be extended to append the data.
 *
 * @return An error code.
 */
int caterva_append(caterva_array_t *array, void *buffer, int64_t buffersize,
                   const int8_t axis);

/**
 * @brief Delete shrinking the given axis delete_len items.
 *
 * @param array The array to shrink.
 * @param axis The axis to shrink.
 * @param delete_start The start position from the axis to start deleting chunks.
 * @param delete_len The number of items to delete to the array->shape[axis].
 * The newshape[axis] will be the old array->shape[axis] - delete_len
 *
 * @return An error code.
 *
 * @note See also caterva_resize
 */
int caterva_delete(caterva_array_t *array, const int8_t axis,
                   int64_t delete_start, int64_t delete_len);


// Indexing section
int caterva_get_orthogonal_selection(caterva_array_t *array, int64_t **selection, int64_t *selection_size, void *buffer,
                                     int64_t *buffershape, int64_t buffersize);

int caterva_set_orthogonal_selection(caterva_array_t *array, int64_t **selection, int64_t *selection_size, void *buffer,
                                     int64_t *buffershape, int64_t buffersize);


// Metainfo section
int32_t caterva_serialize_meta(int8_t ndim, int64_t *shape, const int32_t *chunkshape,
                               const int32_t *blockshape, uint8_t **smeta);

BLOSC_EXPORT int32_t caterva_deserialize_meta(uint8_t *smeta, int32_t smeta_len, int8_t *ndim,
                                              int64_t *shape, int32_t *chunkshape,
                                              int32_t *blockshape);


#ifdef __cplusplus
}
#endif

#endif  // CATERVA_CATERVA_H_
