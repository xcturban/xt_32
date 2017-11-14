

typedef struct TAG_STRUCT_INI{
	FILE *fp;
	const char *fname;
}STRUCT_INI;


STRUCT_INI *ini_new(const char *str_ini);
int ini_release(STRUCT_INI *fp_ini);
char *ini_put_value_string(STRUCT_INI *fp_ini, char *sec, char *key, char *value);
char *ini_get_value_string(STRUCT_INI *fp_ini, char *value, char *sec, char *key);
