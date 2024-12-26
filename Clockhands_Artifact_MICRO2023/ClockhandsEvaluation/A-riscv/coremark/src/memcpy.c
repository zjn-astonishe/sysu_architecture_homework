void* memcpy( void* restrict dst, const void* restrict src, unsigned long count ) {
	for( char* d = dst; count--; ++d ) {
		*d = *(char*)src++;
	}
	return dst;
}
