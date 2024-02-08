#pragma once 

#include "defines.h"

typedef enum memory_tag {
  // TEMPORARY USE ONLY
  MEM_TAG_UNKWN,
  MEM_TAG_ARRAY,
  MEM_TAG_DARRAY,
  MEM_TAG_DICT,
  MEM_TAG_RING_QUEUE,
  MEM_TAG_BST,
  MEM_TAG_STRING,
  MEM_TAG_APPLICATION,
  MEM_TAG_JOB,
  MEM_TAG_TEXTURE,
  MEM_TAG_MATERIAL_INSTANCE,
  MEM_TAG_RENDERER,
  MEM_TAG_GAME,
  MEM_TAG_TRANSFORM,
  MEM_TAG_ENTITY,
  MEM_TAG_ENTITY_NODE,
  MEM_TAG_SCENE,
  MEM_TAG_MAX_TAGS
} memory_tag;

LAPI void initialize_memory();
LAPI void shutdown_memory();

LAPI void* lallocate(u64 size, memory_tag memory_tag);

LAPI void lfree(void* block, u64 size, memory_tag memory_tag);

LAPI void* lzero_memory(void* block, u64 size);

LAPI void* lcopy_memory(void* dest, const void* source, u64 size);

LAPI void* lset_memory(void* dest, i32 value, u64 size);

LAPI char* get_memory_usage_str();
