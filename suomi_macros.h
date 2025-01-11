// use to generate hash table functions of fixed type keys and values, keys cannot by string literals because i don't want to conditionally include string.h, could be a fixed size char buffer though
#define SM_GEN_HASH_TABLE_INTERFACE(interface_prefix, key_type, value_type)
