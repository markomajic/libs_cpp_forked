set(src_dir "${CMAKE_CURRENT_SOURCE_DIR}/bz2_src")

# Explicit library sources only: mk251.c and other utilities define main().
add_compression_libraries(libboost_bzip2 "${src_dir}"
    "${boost_src_dir}/bzip2.cpp"
    "${src_dir}/blocksort.c"
    "${src_dir}/bzlib.c"
    "${src_dir}/compress.c"
    "${src_dir}/crctable.c"
    "${src_dir}/decompress.c"
    "${src_dir}/huffman.c"
    "${src_dir}/int_err.c"
    "${src_dir}/randtable.c"
)
