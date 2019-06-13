
 This is the interface to read/write IEEE Common Data Format file, 
 and will be used as an interface to save/load case.
 
 To simplify the usage, we just define some global variable and the 
 caller should free it at last. they are:
 
 _xieee_bus    *_xieee_cdf_buf;    size_t _xieee_cdf_bus_size;
 _xieee_zone   *_xieee_cdf_zone;   size_t _xieee_cdf_zone_size;
 _xieee_ichg   *_xieee_cdf_ichg;   size_t _xieee_cdf_ichg_size;
 _xieee_line   *_xieee_cdf_line;   size_t _xieee_cdf_line_size;
 _xieee_branch *_xieee_cdf_branch; size_t _xieee_cdf_branch_size;
 
 The title section is only one in a CDF file, so we use one stack variable
 _xieee_title _xieee_cdf_title
