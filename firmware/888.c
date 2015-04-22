#include <REG52.H>
 #include <intrins.h>
#define uchar unsigned char

#define uint unsigned int

uchar display[8][8];

/*rank:A,1,2,3,4,I,心,U*/

uchar code table_cha[8][8]={0x51,0x51,0x51,0x4a,0x4a,0x4a,0x44,0x44,0x18,0x1c,0x18,0x18,0x18,0x18,0x18,0x3c,0x3c,0x66,0x66,0x30,0x18,0xc,0x6,0xf6,0x3c,0x66,0x60,0x38,0x60,0x60,0x66,0x3c,0x30,0x38,0x3c,0x3e,0x36,0x7e,0x30,0x30,0x3c,0x3c,0x18,0x18,0x18,0x18,0x3c,0x3c,0x66,0xff,0xff,0xff,0x7e,0x3c,0x18,0x18,0x66,0x66,0x66,0x66,0x66,0x66,0x7e,0x3c};

/*the "ideasoft"*/

uchar code table_id[40]={0x81,0xff,0x81,0x00,0xff,0x81,0x81,0x7e,0x00,0xff,0x89,0x89,0x00,0xf8,0x27,0x27,0xf8,0x00,0x8f,0x89,0x89,0xf9,0x00,0xff,0x81,0x81,0xff,0x00,0xff,0x09,0x09,0x09,0x01,0x0,0x01,0x01,0xff,0x01,0x01,0x00};

/*railway*/

uchar code dat[128]={0x0,0x20,0x40,0x60,0x80,0xa0,0xc0,0xe0,0xe4,0xe8,0xec,0xf0,0xf4,0xf8,0xfc,0xdc,0xbc,0x9c,0x7c,0x5c,0x3c,0x1c,0x18,0x14,0x10,0xc,0x8,0x4,0x25,0x45,0x65,0x85,0xa5,0xc5,0xc9,0xcd,0xd1,0xd5,0xd9,0xb9,0x99,0x79,0x59,0x39,0x35,0x31,0x2d,0x29,0x4a,0x6a,0x8a,0xaa,0xae,0xb2,0xb6,0x96,0x76,0x56,0x52,0x4e,0x6f,0x8f,0x93,0x73,0x6f,0x8f,0x93,0x73,0x4a,0x6a,0x8a,0xaa,0xae,0xb2,0xb6,0x96,0x76,0x56,0x52,0x4e,0x25,0x45,0x65,0x85,0xa5,0xc5,0xc9,0xcd,0xd1,0xd5,0xd9,0xb9,0x99,0x79,0x59,0x39,0x35,0x31,0x2d,0x29,0x0,0x20,0x40,0x60,0x80,0xa0,0xc0,0xe0,0xe4,0xe8,0xec,0xf0,0xf4,0xf8,0xfc,0xdc,0xbc,0x9c,0x7c,0x5c,0x3c,0x1c,0x18,0x14,0x10,0xc,0x8,0x4};

/*railway 2*/

uchar code dat2[28]={0x0,0x20,0x40,0x60,0x80,0xa0,0xc0,0xe0,0xe4,0xe8,0xec,0xf0,0xf4,0xf8,0xfc,0xdc,0xbc,0x9c,0x7c,0x5c,0x3c,0x1c,0x18,0x14,0x10,0xc,0x8,0x4};

/*railway 3*/

uchar code dat3[24]={0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x16,0x26,0x36,0x46,0x56,0x66,0x65,0x64,0x63,0x62,0x61,0x60,0x50,0x40,0x30,0x20,0x10};

/*3p char*/

uchar code table_3p[3][8]={0xff,0x89,0xf5,0x93,0x93,0xf5,0x89,0xff,0x0e,0x1f,0x3f,0x7e,0x7e,0x3f,0x1f,0x0e,0x18,0x3c,0x7e,0xff,0x18,0x18,0x18,0x18};

/*initializtion

That is to initialize the program .

It is write to set the timer in c52 mcu.

So the program can renovate the led_3d_cube in fixed time use the interrupt function.*/

void sinter()

{

         IE=0x82;

         TCON=0x01;

         TH0=0xc0;

         TL0=0;

         TR0=1;

}

void delay5us(void)   //误差 -0.026765046296us STC 1T 22.1184Mhz
{
    unsigned char a,b;
    for(b=7;b>0;b--)
        for(a=2;a>0;a--);
}

void delay(uint i)

{                                                                                                     

         while (i--){
		 delay5us();}//12t的mcu 注释这个延时即可
}

/*To judge the num bit*/

uchar judgebit(uchar num,uchar b)

{

         char n;

         num=num&(1<<b);

         if (num)

         n=1;

         else

         n=0;

         return n;

}

/*To figure out the round number*/

uchar abs(uchar a)

{

         uchar b;

         b=a/10;

         a=a-b*10;

         if (a>=5)

         b++;

         return b;

}

/*To figure out the absolute value*/

uchar abss(char a)

{

         if (a<0)

         a=-a;

         return a;

}

/*The function can comparat the character.

And remove the big one to the back.*/

void max(uchar *a,uchar *b)

{

   uchar t;

   if ((*a)>(*b))

   {

                   t=(*a);

                   (*a)=(*b);

                   (*b)=t;   

   }

}

/*The function is to figure out the max number and return it.*/

uchar maxt(uchar a,uchar b,uchar c)

{

         if (a<b)

         a=b;

         if (a<c)

         a=c;

         return a;

}

void clear(char le)

{

         uchar i,j;

         for (j=0;j<8;j++)

         {

                   for (i=0;i<8;i++)

                   display[j][i]=le;

         }

}

void trailler(uint speed)

{

         char i,j;

         for (i=6;i>=-3;i--)

         {

                   if (i>=0)

                   {

                            for (j=0;j<8;j++)

                            display[j][i]=display[j][i+1];

                   }

                   if (i<4)

                   {

                            for (j=0;j<8;j++)

                            display[j][i+4]=0;

                   }

                   delay(speed);

         }

}

void point(uchar x,uchar y,uchar z,uchar le)

{

         uchar ch1,ch0;

         ch1=1<<x;

         ch0=~ch1;

         if (le) 

         display[z][y]=display[z][y]|ch1;

         else 

         display[z][y]=display[z][y]&ch0;

}

void type(uchar cha,uchar y)

{        

         uchar xx;

         for (xx=0;xx<8;xx++)

         {

         display[xx][y]=table_cha[cha][xx];

         }

}

/*The first variable is the distance from the midpoint.

The second is the layer.

the third is the flash speed of the time between each two point.

The forth is the enable io,it controls weather draw or claen.*/

void cirp(char cpp,uchar dir,uchar le)

{

         uchar a,b,c,cp;

         if ((cpp<128)&(cpp>=0))

         {

                   if (dir)

                   cp=127-cpp;

                   else 

                   cp=cpp; 

                   a=(dat[cp]>>5)&0x07;

                   b=(dat[cp]>>2)&0x07;

                   c=dat[cp]&0x03;

                   if (cpp>63)

                   c=7-c;

                   point (a,b,c,le);

         }

}

void line(uchar x1,uchar y1,uchar z1,uchar x2,uchar y2,uchar z2,uchar le)

{

         char t,a,b,c,a1,b1,c1,i;

         a1=x2-x1;

         b1=y2-y1;

         c1=z2-z1;

         t=maxt(abss(a1),abss(b1),abss(c1));

         a=x1*10;

         b=y1*10;

         c=z1*10;

         a1=a1*10/t;

         b1=b1*10/t;

         c1=c1*10/t;

         for (i=0;i<t;i++)

         {

                   point(abs(a),abs(b),abs(c),le);

                   a+=a1;

                   b+=b1;

                   c+=c1;

         }

         point(x2,y2,z2,le);

}

void box(uchar x1,uchar y1,uchar z1,uchar x2,uchar y2,uchar z2,uchar fill,uchar le)

{

         uchar i,j,t=0;

         max(&x1,&x2);

         max(&y1,&y2);

         max(&z1,&z2);

         for (i=x1;i<=x2;i++)

                   t|=1<<i;

         if (!le)

                   t=~t;

         if (fill)

         {

                   if (le)

                   {

                            for (i=z1;i<=z2;i++)

                            {

                                     for (j=y1;j<=y2;j++)

                                               display[j][i]|=t;

                            }

                   }

                   else

                   {

                            for (i=z1;i<=z2;i++)

                            {

                                     for (j=y1;j<=y2;j++)

                                               display[j][i]&=t;

                            }

                   }

         }

         else

         {

                   if (le)

                   {

                            display[y1][z1]|=t;

                            display[y2][z1]|=t;

                            display[y1][z2]|=t;

                            display[y2][z2]|=t;

                   }

                   else

                   {

                           display[y1][z1]&=t;

                            display[y2][z1]&=t;

                            display[y1][z2]&=t;

                            display[y2][z2]&=t;

                   }

                   t=(0x01<<x1)|(0x01<<x2);

                   if (!le)

                            t=~t;

                   if (le)

                   {

                            for (j=z1;j<=z2;j+=(z2-z1))

                            {

                                     for (i=y1;i<=y2;i++)

                                               display[i][j]|=t;

                            }

                            for (j=y1;j<=y2;j+=(y2-y1))

                            {

                                     for (i=z1;i<=z2;i++)

                                               display[j][i]|=t;

                            }

                   }

                   else

                   {

                            for (j=z1;j<=z2;j+=(z2-z1))

                            {

                                     for (i=y1;i<=y2;i++)

                                     {

                                     display[i][j]&=t;

                                     }

                            }

                            for (j=y1;j<=y2;j+=(y2-y1))

                            {

                                     for (i=z1;i<=z2;i++)

                                     {

                                     display[j][i]&=t;

                                     }

                            }

                   }

         }

}

void box_apeak_xy(uchar x1,uchar y1,uchar z1,uchar x2,uchar y2,uchar z2,uchar fill,uchar le)

{

         uchar i;

         max(&z1,&z2);

         if (fill)

         {

                   for (i=z1;i<=z2;i++)

                   {

                            line (x1,y1,i,x2,y2,i,le);

                   } 

         } 

         else

         {

                   line (x1,y1,z1,x2,y2,z1,le);

                   line (x1,y1,z2,x2,y2,z2,le);

                   line (x2,y2,z1,x2,y2,z2,le);

                   line (x1,y1,z1,x1,y1,z2,le);

         }

}

void poke(uchar n,uchar x,uchar y)

{

         uchar i;

         for (i=0;i<8;i++)

         {

          point(x,y,i,judgebit(n,i));

         }

}

void boxtola(char i,uchar n)

{

         if ((i>=0)&(i<8))

         {

                   poke(n,0,7-i);

         }

         if ((i>=8)&(i<16))

         {

                   poke(n,i-8,0);

         }

         if ((i>=16)&(i<24))

         {

                   poke(n,7,i-16);

         }

 

}

void rolldisplay(uint speed)

{

         uchar j;

         char i,a;

         for (i=23;i>-40;i--)

         {

                   for (j=0;j<40;j++)

                   {

                            a=i+j;

                            if ((a>=0)&(a<24))

                            boxtola(a,table_id[j]);

                   }

                   delay(speed);

         }

}

void roll_apeak_yz(uchar n,uint speed)

{

         uchar i;

         switch(n)

         {

                   case 1:

                            for (i=0;i<7;i++)

                            {

                                     display[i][7]=0;

                                     display[7][6-i]=255;

                                     delay(speed);

                            };

                            break;

                   case 2:

                            for (i=0;i<7;i++)

                            {

                                     display[7][7-i]=0;

                                     display[6-i][0]=255;

                                     delay(speed);

                            };

                            break;

                   case 3:

                            for (i=0;i<7;i++)

                            {

                                     display[7-i][0]=0;

                                     display[0][i+1]=255;

                                     delay(speed);

                            };

                            break;

                   case 0:

                            for (i=0;i<7;i++)

                            {

                                     display[0][i]=0;

                                     display[i+1][7]=255;

                                     delay(speed);

                            };

         }

}

void roll_apeak_xy(uchar n,uint speed)

{

         uchar i;

         switch(n)

         {

                   case 1:

                            for (i=0;i<7;i++)

                            {

                                     line(0,i,0,0,i,7,0);

                                     line(i+1,7,0,i+1,7,7,1);

                                     delay(speed);

                            };

                            break;

                   case 2:

                            for (i=0;i<7;i++)

                            {

                                     line(i,7,0,i,7,7,0);

                                     line(7,6-i,0,7,6-i,7,1);

                                     delay(speed);

                            };

                            break;

                   case 3:

                            for (i=0;i<7;i++)

                            {

                                     line(7,7-i,0,7,7-i,7,0);

                                     line(6-i,0,0,6-i,0,7,1);

                                     delay(speed);

                            };

                            break;

                   case 0:

                            for (i=0;i<7;i++)

                            {

                                     line(7-i,0,0,7-i,0,7,0);

                                     line(0,i+1,0,0,i+1,7,1);

                                     delay(speed);

                            };

         }

}

void roll_3_xy(uchar n,uint speed)

{

         uchar i;

         switch(n)

         {

                   case 1:

                            for (i=0;i<8;i++)

                            {

                                     box_apeak_xy (0,i,0,7,7-i,7,1,1);

                                     delay(speed);

                                     if (i<7)

                                     box_apeak_xy (3,3,0,0,i,7,1,0);

                            };

                            break;

                   case 2:

                            for (i=0;i<8;i++)

                            {

                                      box_apeak_xy (7-i,0,0,i,7,7,1,1);

                                     delay(speed);

                                     if (i<7)

                                     box_apeak_xy (3,4,0,i,7,7,1,0);

                            };

                            break;

                   case 3:

                            for (i=0;i<8;i++)

                            {

                                     box_apeak_xy (0,i,0,7,7-i,7,1,1);

                                     delay(speed);

                                     if (i<7)

                                     box_apeak_xy (4,4,0,7,7-i,7,1,0);

                            };

                            break;

                   case 0:

                            for (i=0;i<8;i++)

                            {

                                     box_apeak_xy (7-i,0,0,i,7,7,1,1);

                                     delay(speed);

                                     if (i<7)

                                     box_apeak_xy (4,3,0,7-i,0,7,1,0);

                            };

         }

}

void trans(uchar z,uint speed)

{

         uchar i,j;

         for (j=0;j<8;j++)

         {

                   for (i=0;i<8;i++)

                   {

                            display[z][i]>>=1;

                   }

                   delay(speed);

         }

}

void tranoutchar(uchar c,uint speed)

{

         uchar i,j,k,a,i2=0;

         for (i=0;i<8;i++)

         {

                   if (i<7)

                   box_apeak_xy (i+1,0,0,i+1,7,7,1,1);

                   box_apeak_xy (i2,0,0,i2,7,7,1,0);

                   a=0;

                   i2=i+1;

                   for (j=0;j<=i;j++)

                   {

                            a=a|(1<<j);

                   }

                   for (k=0;k<8;k++)

                   {

                            display[k][3]|=table_cha[c][k]&a;

                            display[k][4]|=table_cha[c][k]&a;

                   }

                   delay(speed);

         }

}

void transss()

{

         uchar i,j;

         for (i=0;i<8;i++)

         {

                   for (j=0;j<8;j++)

                   display[i][j]<<=1;

         }

}

 

/*From now on,the function below is to display the flash.*/

void flash_1()

{

 

         clear(0);

         type(1,0);

         delay(60000);

         type(2,0);

         delay(60000);

         type(3,0);

         delay(60000);

         type(4,0);

         delay(60000);

         delay(60000);

         clear(0);

         rolldisplay(30000);

         type(0,7);

         delay(60000);

         trailler(6000);

         delay(60000);

}

void flash_2()

{

         uchar i;

         for (i=129;i>0;i--)

         {

                   cirp(i-2,0,1);

                   delay(8000);

                   cirp(i-1,0,0);

         }

         delay(8000);     

         for (i=0;i<136;i++)

         {

                   cirp(i,1,1);

                   delay(8000);

                   cirp(i-8,1,0);

         }

         delay(8000);

                  for (i=129;i>0;i--)

         {

                   cirp(i-2,0,1);

                   delay(8000);

         }

         delay(8000);

                   for (i=0;i<128;i++)

         {

                   cirp(i-8,1,0);

                   delay(8000);

         }

         delay(60000);  

}

void flash_3()

{

         char i;

         for (i=0;i<8;i++)

         {

                   box_apeak_xy(0,i,0,7,i,7,1,1);

                   delay(20000);

                   if (i<7)

                   box_apeak_xy(0,i,0,7,i,7,1,0);

         }

         for (i=7;i>=0;i--)

         {

                   box_apeak_xy(0,i,0,7,i,7,1,1);

                   delay(20000);

                   if (i>0)

                   box_apeak_xy(0,i,0,7,i,7,1,0);

         }

         for (i=0;i<8;i++)

         {

                   box_apeak_xy(0,i,0,7,i,7,1,1);

                   delay(20000);

                   if (i<7)

                   box_apeak_xy(0,i,0,7,i,7,1,0);

         }

}

void flash_4()

{

         char i,j,an[8];

         for (j=7;j<15;j++)

                   an[j-7]=j;

         for (i=0;i<=16;i++)

         {

                   for (j=0;j<8;j++)

                   {

                            if ((an[j]<8)&(an[j]>=0))

                            line(0,an[j],j,7,an[j],j,1);

                   }

                   for (j=0;j<8;j++)

                   {

                            if (((an[j]+1)<8)&(an[j]>=0))

                            line(0,an[j]+1,j,7,an[j]+1,j,0);

                   }

                   for (j=0;j<8;j++)

                   {

                            if (an[j]>0)

                            an[j]--;

                   }

                   delay(15000);

         }

         for (j=0;j<8;j++)

                   an[j]=1-j;

         for (i=0;i<=16;i++)

         {

                   for (j=0;j<8;j++)

                   {

                            if ((an[j]<8)&(an[j]>=0))

                            line(0,an[j],j,7,an[j],j,1);

                   }

                   for (j=0;j<8;j++)

                   {

                            if (((an[j]-1)<7)&(an[j]>0))

                            line(0,an[j]-1,j,7,an[j]-1,j,0);

                   }

                   for (j=0;j<8;j++)

                   {

                            if (an[j]<7)

                            an[j]++;

                   }

                   delay(15000);

         }

}

void flash_5()

{

         uint a=15000;//a=delay

         char i=8,j,an[4];

         //1

         for (j=7;j<11;j++)

                   an[j-7]=j;

         while(i--)

         {

                   for (j=0;j<4;j++)

                   {

                            if (an[j]<8)

                            box_apeak_xy(j,an[j],j,7-j,an[j],7-j,0,1);

                            if (an[j]<7)

                            box_apeak_xy(j,an[j]+1,j,7-j,an[j]+1,7-j,0,0);

                   }

                   for (j=0;j<4;j++)

                   {

                            if (an[j]>3)

                            an[j]--;

                   }

                   delay(a);

         }

         //2

         i=3;

         for (j=0;j<4;j++)

         an[j]=5-j;

         while(i--)

         {

                   for (j=1;j<4;j++)

                   {

                            if (an[j]<4)

                            box_apeak_xy(j,an[j],j,7-j,an[j],7-j,0,1);

                            if (an[j]<3)

                            box_apeak_xy(j,an[j]+1,j,7-j,an[j]+1,7-j,0,0);

                   }

                   for (j=0;j<4;j++)

                   {

                            if (an[j]>0)

                            an[j]--;

                   }

                   delay(a);

         }

         //3

         i=3;

         for (j=1;j<4;j++)

         an[j]=4-j;

         while(i--)

         {

                   for (j=1;j<4;j++)

                   {

                            if (an[j]>=0)

                            box_apeak_xy(j,an[j],j,7-j,an[j],7-j,0,1);

                            if (an[j]>0)

                            box_apeak_xy(j,an[j]-1,j,7-j,an[j]-1,7-j,0,0);

                   }

                   for (j=1;j<4;j++)

                   {

                            if (an[j]<3)

                            an[j]++;

                   }

                   delay(a);

         }

 

         //4

         i=3;

         for (j=0;j<4;j++)

         an[j]=j+1;

         while(i--)

         {

                   for (j=1;j<4;j++)

                   {

                            if (an[j]>3)

                            box_apeak_xy(j,an[j],j,7-j,an[j],7-j,0,1);

                            if (an[j]>3)

                            box_apeak_xy(j,an[j]-1,j,7-j,an[j]-1,7-j,0,0);

                   }

                   for (j=0;j<4;j++)

                            an[j]++;

                   delay(a);

         }

         //5

         i=3;

         for (j=3;j<6;j++)

                   an[j-2]=j;

         while(i--)

         {

                   for (j=1;j<4;j++)

                   {

                            box_apeak_xy(j,an[j],j,7-j,an[j],7-j,0,1);

                            box_apeak_xy(j,an[j]+1,j,7-j,an[j]+1,7-j,0,0);

                   }

                   for (j=0;j<4;j++)

                   {

                            if (an[j]>3)

                            an[j]--;

                   }

                   delay(a);

         }

         //6

         i=3;

         for (j=0;j<4;j++)

         an[j]=5-j;

         while(i--)

         {

                   for (j=1;j<4;j++)

                   {

                            if (an[j]<4)

                            box_apeak_xy(j,an[j],j,7-j,an[j],7-j,0,1);

                            if (an[j]<3)

                            box_apeak_xy(j,an[j]+1,j,7-j,an[j]+1,7-j,0,0);

                   }

                   for (j=0;j<4;j++)

                   {

                            if (an[j]>0)

                            an[j]--;

                   }

                   delay(a);

         }

         //7

         i=3;

         for (j=0;j<4;j++)

         an[j]=3-j;

         an[0]=2;

         while(i--)

         {

                   for (j=0;j<3;j++)

                   {

                            if (an[j]>=0)

                                     box_apeak_xy(j,an[j],j,7-j,an[j],7-j,0,1);

                            if (an[j]>=0)

                                     box_apeak_xy(j,an[j]+1,j,7-j,an[j]+1,7-j,0,0);

                   }

                   for (j=0;j<4;j++)

                   {

                            if (j<5-i)

                            an[j]--;

                   }

                   delay(a);

         }

         //8

         i=10;

         for (j=0;j<4;j++)

                   an[j]=j-2;

         while(i--)

         {

                   for (j=0;j<4;j++)

                   {

                            if (an[j]>=0)

                            box_apeak_xy(j,an[j],j,7-j,an[j],7-j,0,1);

                            if (an[j]>=0)

                            box_apeak_xy(j,an[j]-1,j,7-j,an[j]-1,7-j,0,0);

                   }

                   for (j=0;j<4;j++)

                   {

                            if (an[j]<7)

                            an[j]++;

                   }

                   delay(a);

         }

}

void flash_6()

{

      uchar i,j,k,z;

      roll_apeak_yz(1,10000);

      roll_apeak_yz(2,10000);

      roll_apeak_yz(3,10000);

      roll_apeak_yz(0,10000);

      roll_apeak_yz(1,10000);

      roll_apeak_yz(2,10000);

      roll_apeak_yz(3,10000);

      for (i=0;i<3;i++)

               {

                         for (j=0;j<8;j++)

                         {

                                  for (k=0;k<8;k++)

                                  {

                                           if ((table_3p[i][j]>>k)&1)

                                           {

                                                     for (z=1;z<8;z++)

                                                              {

                                                                       point (j,7-k,z,1);

                                                                       if (z-1)

                                                                       point (j,7-k,z-1,0);

                                                                       delay(5000);

                                                              }

                                           }

                                  }

                         }

                         trans(7,15000);

               }

}

void flash_7()

{

      uchar i;

      uint a=3000;

      roll_apeak_yz(0,10000);

      roll_apeak_yz(1,10000);

      roll_apeak_yz(2,10000);

      roll_apeak_yz(3,10000);

      roll_apeak_yz(0,10000);

      roll_apeak_yz(1,10000);

      roll_apeak_yz(2,10000);

      roll_apeak_yz(3,10000);

      roll_apeak_yz(0,10000);

      roll_apeak_yz(1,10000);

      roll_apeak_yz(2,10000);

      roll_apeak_xy(0,10000);

      roll_apeak_xy(1,10000);

      roll_apeak_xy(2,10000);

      roll_apeak_xy(3,10000);

      roll_apeak_xy(0,10000);

      roll_apeak_xy(1,10000);

      roll_apeak_xy(2,10000);

      roll_apeak_xy(3,10000);

      for (i=0;i<8;i++)

      {

               box_apeak_xy (0,i,0,7-i,i,7,1,1);

               delay(a);

      }

      delay(30000);

      roll_3_xy(0,a);

      delay(30000);

      roll_3_xy(1,a);

      delay(30000);

      roll_3_xy(2,a);

      delay(30000);

      roll_3_xy(3,a);

      delay(30000);

      roll_3_xy(0,a);

      delay(30000);

      roll_3_xy(1,a);

      delay(30000);

      roll_3_xy(2,a);

      delay(30000);

      roll_3_xy(3,a);

      for (i=7;i>0;i--)

      {

               box_apeak_xy(i,0,0,i,7,7,1,0);

               delay(a);

      }

}

void flash_8()

{

      uchar i;

      for (i=5;i<8;i++)

      {

               tranoutchar(i,10000);

               delay(60000);

               delay(60000);

      }

}

void flash_9()

{

      char i;

      uchar j,an[8],x,y,t,x1,y1;

      for (i=0;i<8;i++)

      {

               box_apeak_xy (i,0,0,i,7,7,1,1);

               if (i)

               box_apeak_xy (i-1,0,0,i-1,7,7,1,0);

               delay(10000);

      }

      roll_apeak_xy(3,10000);

      roll_apeak_xy(0,10000);

      roll_apeak_xy(1,10000);

      for (i=0;i<7;i++)

      {

               line(6-i,6-i,0,6-i,6-i,7,1);

               line(i,7,0,i,7,7,0);

               delay(10000);

      }

      for (i=0;i<8;i++)

               an[i]=14;

      for (i=0;i<85;i++)

      {

               clear(0);

               for (j=0;j<8;j++)

               {

                         t=an[j]%28;

                         x=dat2[t]>>5;

                         y=(dat2[t]>>2)&0x07;

                         t=(an[j]-14)%28;

                         x1=dat2[t]>>5;

                         y1=(dat2[t]>>2)&0x07;

                         line(x,y,j,x1,y1,j,1);

               }

               for (j=0;j<8;j++)

               {

               if ((i>j)&(j>i-71))

               an[j]++;

               } 

               delay(5000);

      }

      for (i=0;i<85;i++)

      {

               clear(0);

               for (j=0;j<8;j++)

               {

                         t=an[j]%28;

                         x=dat2[t]>>5;

                         y=(dat2[t]>>2)&0x07;

                         t=(an[j]-14)%28;

                         x1=dat2[t]>>5;

                         y1=(dat2[t]>>2)&0x07;

                         line(x,y,j,x1,y1,j,1);

               }

               for (j=0;j<8;j++)

               {

               if ((i>j)&(j>i-71))

               an[j]--;

               } 

               delay(5000);

      }

      for (i=0;i<29;i++)

      {

               clear(0);



               t=an[0]%28;

               x=dat2[t]>>5;

               y=(dat2[t]>>2)&0x07;

               t=(an[0]-14)%28;

               x1=dat2[t]>>5;

               y1=(dat2[t]>>2)&0x07;

               box_apeak_xy(x,y,0,x1,y1,7,0,1);

               box_apeak_xy(x,y,1,x1,y1,6,0,1);

               an[0]++; 

               delay(5000);

      }

      for (i=0;i<16;i++)

      {

               clear(0);



               t=an[0]%28;

               x=dat2[t]>>5;

               y=(dat2[t]>>2)&0x07;

               t=(an[0]-14)%28;

               x1=dat2[t]>>5;

               y1=(dat2[t]>>2)&0x07;

               box_apeak_xy(x,y,0,x1,y1,7,1,1);

               an[0]--; 

               delay(5000);

      }

      for (i=0;i<8;i++)

      {

               line(i,i,0,0,0,i,0);

               delay(5000);

      }

      for (i=1;i<7;i++)

      {

               line(i,i,7,7,7,i,0);

               delay(5000);

      }

      for (i=1;i<8;i++)

      {

               clear(0);

               box(7,7,7,7-i,7-i,7-i,0,1);

               delay(10000);

      }

      for (i=1;i<7;i++)

      {

               clear(0);

               box(0,0,0,7-i,7-i,7-i,0,1);

               delay(10000);

      }

      for (i=1;i<8;i++)

      {

               clear(0);

               box(0,0,0,i,i,i,0,1);

               delay(10000);

      }

      for (i=1;i<7;i++)

      {

               clear(0);

               box(7,0,0,i,7-i,7-i,0,1);

               delay(10000);

      }

      for (i=1;i<8;i++)

      {

               box(7,0,0,7-i,i,i,1,1);

               delay(10000);

      }

      for (i=1;i<7;i++)

      {

               clear(0);

               box(0,7,7,7-i,i,i,1,1);

               delay(10000);

      }

}

void flash_10()

{

      uchar i,j,an[4],x,y,t;

      for (i=1;i<7;i++)

      {

               

               clear(0);

               box(0,6,6,1,7,7,1,1);

               box(i,6,6-i,i+1,7,7-i,1,1);

               box(i,6,6,i+1,7,7,1,1);

               box(0,6,6-i,1,7,7-i,1,1);

               box(0,6-i,6,1,7-i,7,1,1);

               box(i,6-i,6-i,i+1,7-i,7-i,1,1);

               box(i,6-i,6,i+1,7-i,7,1,1);

               box(0,6-i,6-i,1,7-i,7-i,1,1);

               delay(30000);

      }

      for (i=0;i<4;i++)

      {

               an[i]=6*i;

      }

      for (i=0;i<35;i++)

      {

               clear(0);

               for(j=0;j<4;j++)

               {

                         t=an[j]%24;

                         x=dat3[t]>>4;

                         y=dat3[t]&0x0f;

                         box(x,y,0,x+1,y+1,1,1,1);

                         box(x,y,6,x+1,y+1,7,1,1);

               }

               for (j=0;j<4;j++)

               an[j]++; 

               delay(10000);

      }

      for (i=0;i<35;i++)

      {

               clear(0);

               for(j=0;j<4;j++)

               {

                         t=an[j]%24;

                         x=dat3[t]>>4;

                         y=dat3[t]&0x0f;

                         box(x,y,0,x+1,y+1,1,1,1);

                         box(x,y,6,x+1,y+1,7,1,1);

               }

               for (j=0;j<4;j++)

               an[j]--; 

               delay(10000);

      }

      for (i=0;i<35;i++)

      {

               clear(0);

               for(j=0;j<4;j++)

               {

                         t=an[j]%24;

                         x=dat3[t]>>4;

                         y=dat3[t]&0x0f;

                         box(x,0,y,x+1,1,y+1,1,1);

                         box(x,6,y,x+1,7,y+1,1,1);

               }

               for (j=0;j<4;j++)

               an[j]++; 

               delay(10000);

      }

      for (i=0;i<36;i++)

      {

               clear(0);

               for(j=0;j<4;j++)

               {

                         t=an[j]%24;

                         x=dat3[t]>>4;

                         y=dat3[t]&0x0f;

                         box(x,0,y,x+1,1,y+1,1,1);

                         box(x,6,y,x+1,7,y+1,1,1);

               }

               for (j=0;j<4;j++)

               an[j]--; 

               delay(10000);

      }

      for (i=6;i>0;i--)

      {

               

               clear(0);

               box(0,6,6,1,7,7,1,1);

               box(i,6,6-i,i+1,7,7-i,1,1);

               box(i,6,6,i+1,7,7,1,1);

               box(0,6,6-i,1,7,7-i,1,1);

               box(0,6-i,6,1,7-i,7,1,1);

               box(i,6-i,6-i,i+1,7-i,7-i,1,1);

               box(i,6-i,6,i+1,7-i,7,1,1);

               box(0,6-i,6-i,1,7-i,7-i,1,1);

               delay(30000);

      }

}

void flash_11()

{

      uchar i,j,t,x,y;

      uchar code daa[13]={0,1,2,0x23,5,6,7,6,5,0x23,2,1,0};

      for (j=0;j<5;j++)

      {

               for (i=0;i<13;i++)

               {

                         if (daa[i]>>4)

                         {

                                  t=daa[i]&0x0f;

                                  line (0,0,t+1,0,7,t+1,1);

                         }

                         else 

                                  t=daa[i];

                         line (0,0,t,0,7,t,1);

                         transss();

                         delay(10000);

               }

      }

      for (j=1;j<8;j++)

      {

               if (j>3)

                         t=4;

               else

                         t=j;

               for (i=0;i<24;i+=j)

               {

                         x=dat3[i]>>4;

                         y=dat3[i]&0x0f;

                         box_apeak_xy(0,x,y,0,x+1,y+1,1,1);

                         transss();

                         delay(10000);

               }

      }

      for (j=1;j<8;j++)

      {

               if (j>3)

                         t=4;

               else

                         t=j;

               for (i=0;i<24;i+=j)

               {

                         x=dat3[i]>>4;

                         y=dat3[i]&0x0f;

                         point (0,x,y,1);

                         transss();

                         delay(10000);

               }

      }

}

void main()

{
		
         sinter();
		 while(1){
        // clear(0);

         /*play list*/

         //flash_1();

         clear(0);

         flash_2();

         flash_3();

         flash_4();

         flash_4();

         flash_5();

      flash_5();

      flash_6();

      flash_7();

      flash_8();

      flash_9();

      flash_10();

      clear (0);

      flash_11();

      flash_9();

      flash_5();

      flash_7();

      flash_5();

      flash_6();

      flash_8();

      flash_9();

      flash_10();

         }

}

//P0;   //573 in
//P1;  //uln2803
//P2;  //573 LE

void print() interrupt 1

{

         uchar i;

         static uchar layer=0;

         P1=0;

         for (i=0;i<8;i++)

         {

                   P2=1<<i;

                   delay(3);

                   P0=display[layer][i];

                   delay(3);

         }

         P1=1<<layer;

         if (layer<7) 

                   layer++;

         else 

                   layer=0;

         TH0=0xc0;

         TL0=0;

}