#include <stdio.h>
#include <stdlib.h>
#include <mraa/aio.h>
#include <mraa.h>
#include <time.h>
#include <math.h>


mraa_aio_context temp_a0;
int B = 4275;
int main()
{
	uint16_t temp_value = 0;
	//FILE *fd = fopen("log.txt","w+");
	time_t t;
	temp_a0 = mraa_aio_init(0);

	int buf_length = 150;
	char buf[buf_length];

	if(temp_a0 == NULL)
		return 1;
	//fprintf(fd,"fuck");
	int fp = open("lab4_1.log", O_WRONLY | O_CREAT);
	//write(fp,"c",sizeof(char));
	while(1)
	{
		temp_value = mraa_aio_read(temp_a0);
		//printf("temp is %d\n",temp_value);
		struct tm *loc_time;
		t = time(NULL);
		loc_time = localtime(&t);
		float R = (1023.0/(float)temp_value) - 1.0;
		R = 100000.0 * R;
		
	        float temperature=1.0/(log(R/100000.0)/B+1/298.15)-273.15;//convert to temperature via datasheet ;	
		temperature = temperature * (9.0/5.0) + 32.0;
		strftime(buf,buf_length,"%H:%M:%S ",loc_time);
		//fprintf(fd,"%s ",buf);
		write(fp, buf, 9);
		char temp_buf[4];
		snprintf(temp_buf, 7, "%2.1f",temperature);
		//fprintf(fd," %0.1f\n",temperature*1.0);
		write(fp, temp_buf, 4);
		write(fp,"\n",1);
		//printf("%s\n",temp_buf);
		//printf("%d:%d:%d ",loc_time->tm_hour,loc_time->tm_min,loc_time->tm_sec);
		sleep(1);
	}
	

return 0;
}
