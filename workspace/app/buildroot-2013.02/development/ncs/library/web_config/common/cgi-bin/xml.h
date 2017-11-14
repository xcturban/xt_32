#ifndef _xml_h
#define _xml_h

FILE *xml_new(char *xml_file_name);

int xml_release(FILE *fp);

char *xml_get_value_string(char *value, char *argv, FILE *fp);
 

#endif /* _xml_h */
