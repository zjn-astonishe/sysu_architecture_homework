int ee_printf(const char*,...);

int puts(const char* str) {
	return ee_printf("%s\n", str);
}
