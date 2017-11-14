all:
	$(CC) -o response_Main.cgi response_Main.c Common.c  xml.c ini.c xml2ini.c -L$(L) 
	$(CC) -o response_x2i.cgi response_x2i.c Common.c  xml.c ini.c xml2ini.c -L$(L) 
	$(CC) -o response_WebCfg.cgi response_WebCfg.c Common.c  xml.c ini.c xml2ini.c -L$(L) 
	$(CC) -o response_Login.cgi response_Login.c Common.c  xml.c ini.c xml2ini.c -L$(L) 
	$(CC) -o response_UploadFile.cgi response_UploadFile.c Common.c  xml.c ini.c xml2ini.c -L$(L) 
	$(CC) -o Log.cgi Log.c Common.c  xml.c ini.c xml2ini.c -L$(L) 
	$(CC) -o iniUpdate iniUpdate.c Common.c  xml.c ini.c -L$(L) 
#this line for testing
#	$(CC) -o response_Test.cgi response_Test.c Common.c  xml.c ini.c xml2ini.c -L$(L) 

clean:
	mv *.cgi rubish
