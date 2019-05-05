
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"yl_show_font.h"

#define TRUE    1
#define FALSE   0

int days_in_month[]={0,31,28,31,30,31,30,31,31,30,31,30,31};
char *months[]=
{
	" ",
	"\n\n\nJanuary",
	"\n\n\nFebruary",
	"\n\n\nMarch",
	"\n\n\nApril",
	"\n\n\nMay",
	"\n\n\nJune",
	"\n\n\nJuly",
	"\n\n\nAugust",
	"\n\n\nSeptember",
	"\n\n\nOctober",
	"\n\n\nNovember",
	"\n\n\nDecember"
};


int inputyear(void)
{
	int year;
	
	printf("Please enter a year (example: 1999) : ");
	scanf("%d", &year);
	return year;
}

int determinedaycode(int year)
{
	int daycode;
	
	int d1, d2, d3;
	
	d1 = (year - 1.)/ 4.0;
	d2 = (year - 1.)/ 100.;
	d3 = (year - 1.)/ 400.;
	daycode = (year + d1 - d2 + d3) %7;
	return daycode;
}


int determineleapyear(int year)
{
	if(year% 4 == FALSE && year%100 != FALSE || year%400 == FALSE)
	{
		days_in_month[2] = 29;
		return TRUE;
	}
	else
	{
		days_in_month[2] = 28;
		return FALSE;
	}
}

void calendar(int year, int daycode)
{
	 
    int month, day,i, k;
    
    char row_rest[29] ;
    char string[10];
    int x;
    int y;
    
	for ( month = 1; month <= 12; month++ )
	{
       lcd_clear_screen(BLACK);
	    memset(row_rest, '\0',sizeof(row_rest));
	    memset(string, '\0',sizeof(string));
	    
        x = 100;
        y = 100;
       // for(i=0;i<10;i++) {string[i]=' ';}
        //printf("55566");
        //for(k=0;k<29;k++) {row_rest[k]=' ';}
	    printf("%s", months[month]);

        //printf("first line");
        lcd_show_string(x, y, months[month]); //显示第一行
	
        y = y + 20;
	      //printf("second line");
        lcd_show_string(x, y, "Sun  Mon  Tue  Wed  Thu  Fri  Sat");//显示第二行
	
        y = y + 20;
		// Correct the position for the first date
		for ( day = 1; day <= 1 + daycode * 5; day++ )
		{
			printf(" ");
            strcat(row_rest, " ");
		}
		
		// Print all the dates for one month
		for ( day = 1; day <= days_in_month[month]; day++ )
		{
			printf("%2d", day );
		   //snprintf(string,10,"%d",day);
		
		  
           snprintf(string, 10,"%2d",day);
            //itoa(day, string, 10);
           strcat(row_rest,string);
            
			// Is day before Sat? Else start next line Sun.
			if ( ( day + daycode ) % 7 > 0 )
            {
                printf("   " );
                strcat(row_rest, "   ");
            }
			else
            {
		        printf("datacode");
             lcd_show_string(x, y, row_rest);
		
                //for(i=0;i<29;i++) {row_rest[i]=' ';}
		        memset(row_rest, '\0',sizeof(row_rest));
			 	   printf("\n " );
              y = y + 20;
                
            }
            
		}
    	//printf("the rest");
        lcd_show_string(x, y, row_rest);
	
        
        // Set position for next month
        daycode = ( daycode + days_in_month[month] ) % 7;
        sleep(2);
        
    }
}

/*
int main(void)
{
	int year, daycode, leapyear;
	
	year = inputyear();
	daycode = determinedaycode(year);
	determineleapyear(year);
	calendar(year, daycode);
	printf("\n");
}*/
int main(int argc, char *argv[])
{
    
    
    int year, daycode, leapyear; //日历
    
    int ret;
    
    year = inputyear();
    //printf("wjc okkkkk");
    daycode = determinedaycode(year);
    determineleapyear(year);
    
    /* “‘ø…∂¡ø…–¥∑Ω Ω¥Úø™LCD“∫æß«˝∂Ø */
    fd_fb = open("/dev/fb0", O_RDWR);
    if(fd_fb == -1)
    {
        printf("can't open /dev/fb0!\n");
        return -1;
    }
    
    /* ªÒ»°LCD“∫æßµƒø…±‰≤Œ ˝ */
    ret = ioctl(fd_fb, FBIOGET_VSCREENINFO, &var);
    if(ret == -1)
    {
        printf("can't ioctl for /dev/fb0!\n");
        return -1;
    }
    
    /* ªÒ»°LCD“∫æßµƒπÃ∂®≤Œ ˝ */
    ret = ioctl(fd_fb, FBIOGET_FSCREENINFO, &fix);
    if(ret == -1)
    {
        printf("can't ioctl for /dev/fb0!\n");
        return -1;
    }
    
    /* ªÒ»°“∫æßœ‘¥Ê£¨√ø“ª––œ‘¥Ê£¨√ø“ª∏ˆœÒÀÿœ‘¥Êµƒ¥Û–° */
    screen_size = var.xres * var.yres * var.bits_per_pixel / 8;
    line_width  = var.xres * var.bits_per_pixel / 8;
    pixel_width = var.bits_per_pixel / 8;
    
    /* Ω´“∫æßœ‘¥Ê”≥…‰µΩ”√ªßø’º‰ */
    fbmem = mmap(NULL, screen_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_fb, 0);
    if(fbmem == (char *)-1)
    {
        printf("mmap for /dev/fb0 error!\n");
        return -1;
    }
    
    /* “‘÷ª∂¡∑Ω Ω¥Úø™∫∫◊”ø‚HZK16’‚∏ˆŒƒº˛ */
    fd_hzk16 = open("HZK16", O_RDONLY);
    if(fd_hzk16 == -1)
    {
        printf("can't open HZK16!\n");
        return -1;
    }
    
    /* ªÒ»°HZk16’‚∏ˆ∫∫◊÷ø‚µƒŒƒº˛–≈œ¢ */
    ret = fstat(fd_hzk16, &hzk16_stat);
    if(ret == -1)
    {
        printf("fstat for HZK16 is error!\n");
        return -1;
    }
    
    /* Ω´∫∫◊”ø‚HZK16Œƒº˛÷–µƒƒ⁄»›”≥…‰µΩ”√ªßø’º‰ */
    hzk16mem = mmap(NULL, hzk16_stat.st_size, PROT_READ, MAP_SHARED, fd_hzk16, 0);
    if(hzk16mem == (char *)-1)
    {
        printf("mmap for HZK16 error!\n");
        return -1;
    }
    
    /* Ω´’˚∏ˆ“∫æß∆¡«Â∆¡Œ™∫⁄…´ */
    lcd_clear_screen(BLACK);
    // printf("jhhjhhjh");
    
    /* ‘⁄“∫æß∆¡…œΩ¯––÷–”¢Œƒµƒœ‘ æ≤Ÿ◊˜ */
    /*lcd_show_string(100, 100, "Hello world!");
    lcd_show_string(100, 120, "Welcome to Embedded world!");
    lcd_show_chinese(100, 140, "—ßœ∞«∂»Î Ωø™∑¢");
    lcd_show_str_chn(100, 160, "…Ú—Ù≈©“µ¥Û—ß, www.syau.edu.cn");
    */
    calendar(year, daycode);
    // printf("jkjkji");
    
    /* Ω‚≥˝Œƒº˛‘⁄”√ªßø’º‰µƒ”≥…‰ */
    munmap(fbmem, screen_size);
    munmap(hzk16mem, hzk16_stat.st_size);
    
    /* Õ®π˝Œƒº˛æ‰±˙πÿ±’¥Úø™µƒŒƒº˛ */
    close(fd_fb);
    close(fd_hzk16);
    
    return 0;
}
