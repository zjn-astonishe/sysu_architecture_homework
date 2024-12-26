void* memset( void* str, int chr, unsigned long len ) {
	for( char* s = str; len--; ++s ) {
		*s = (char)chr;
	}
	return str;
}
