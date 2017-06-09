#include <global.h>

extern char *filename;

void log_write(const char *fmt, ...)
{
    FILE *fp_log;
    char buff[20];

    struct tm *sTm;
    
    time_t now = time (0);
    sTm = gmtime (&now);
    strftime (buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", sTm);

    fp_log = fopen("log.txt", "a+");

    if(fp_log == NULL)
    {
	printf("[LOG] log file open failed\n");
	exit(0);
    }

    va_list args;

    if(comm_str.debug_stdout)
    {
	va_start(args, fmt);
//	fprintf(stdout, "%s ", buff);
	vfprintf(stdout, fmt, args);
	fflush(stdout);
	va_end(args);
    }
    
    va_start(args, fmt);
//    fprintf(fp_log, "%s ", buff);
    vfprintf(fp_log, fmt, args);
    fflush(fp_log);
    fclose(fp_log);

    va_end(args);
}

void print_payload(unsigned char* data , int len)
{
    for(int i=0 ; i < len ; i++)
    {
        if(i != 0 && i%16 == 0)   //if one line of hex printing is complete...
        {
            TRACE(("         "));
            for(int j = i-16; j < i; j++)
            {
                if(data[j] >= 32 && data[j] <= 128)
                    TRACE(("%c",(unsigned char)data[j])); //if its a number or alphabet
                 
                else TRACE((".")); //otherwise print a dot
            }
            TRACE(("\n"));
        } 
         
        if(i%16 == 0) 
	    TRACE(("   "));
	TRACE((" %02X",(unsigned int)data[i]));
                 
        if(i == len-1)  //print the last spaces
        {
            for(int j = 0; j < 15 - i%16; j++) 
		TRACE(("   ")); //extra spaces
             
            TRACE(("         "));
             
            for(int j = i-i%16; j <= i; j++)
            {
                if(data[j] >= 32 && data[j] <= 128) 
		    TRACE(("%c",(unsigned char)data[j]));
                else 
		    TRACE(("."));
            }
            TRACE(("\n"));
        }
    }
}
