char *get_value(char *value, char *argv, char *argv_sub, char div_char, int str_len);
int get_value_from_xml(char *value, char *file_name_xml, char *argv);
int cgi_to_html(char *file_name_obj, char *file_name_src, char *argv_table, char div_char);
int cgi_to_browser(char *file_name_src, char *argv_table, char div_char); 
int cgi_to_browser_by_xml(char *file_name_src, char *file_name_xml);
int cgi_to_browser_by_ini(char *file_name_src, char *file_name_ini);
int lan_to_cgi_by_ini(char *file_name_obj, char *file_name_src, char *file_name_ini);
int cgi_to_html_by_ini(char *file_name_obj, char *file_name_src, char *file_name_ini);
int x2i_to_ini(char *file_name_obj, char *file_name_src, char *data);
int linuxSystem(char *cmdStr);
//int gb23122utf8(char *outstr, char *instr);
char *urlEncode(char *outstr, char *instr, int str_len);
char *urlDecode(char *outstr, char *instr, int str_len);
