#include "GUI.h"
#include "includes.h"

uint16_t COLOR = FK_COLOR;
uint16_t BKCOLOR = BK_COLOR;


void LCD_SetWindow(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey)
{
  LCD_WR_REG(0x2A); 
  LCD_WR_DATA(sx>>8);LCD_WR_DATA(sx&0xFF);
  LCD_WR_DATA(ex>>8);LCD_WR_DATA(ex&0xFF);
  LCD_WR_REG(0x2B); 
  LCD_WR_DATA(sy>>8);LCD_WR_DATA(sy&0xFF);
  LCD_WR_DATA(ey>>8);LCD_WR_DATA(ey&0xFF);
}

void GUI_SetColor(uint16_t color)
{
  COLOR=color;
}

void GUI_SetBkColor(uint16_t bkcolor)
{
  BKCOLOR=bkcolor;
}

uint16_t GUI_GetColor(void)
{
  return COLOR;
}

uint16_t GUI_GetBkColor(void)
{
  return BKCOLOR;
}

void GUI_Clear(uint16_t color)
{
  uint32_t index=0;    
  LCD_SetWindow(0, 0, LCD_WIDTH-1, LCD_HEIGHT-1);
  LCD_WR_REG(0x2C);  
  for(index=0; index<LCD_WIDTH*LCD_HEIGHT; index++)
  {
    LCD_WR_16BITS_DATA(color);
  }
}  

static uint8_t pixel_limit_flag = 0;
static GUI_RECT pixel_limit_rect;

void GUI_SetRange(int16_t x0, int16_t y0, int16_t x1, int16_t y1)
{
  pixel_limit_flag = 1;
  pixel_limit_rect.x0 = x0;
  pixel_limit_rect.x1 = x1;
  pixel_limit_rect.y0 = y0;
  pixel_limit_rect.y1 = y1;
}

void GUI_CancelRange(void)
{
  pixel_limit_flag = 0;
}

void GUI_DrawPixel(int16_t x, int16_t y, uint16_t color)
{	   
  if(pixel_limit_flag == 1
    &&(x <  pixel_limit_rect.x0
     ||x >= pixel_limit_rect.x1
     ||y <  pixel_limit_rect.y0
     ||y >= pixel_limit_rect.y1))
    return ;
  
  LCD_SetWindow(x, y, x, y);		 	 
  LCD_WR_REG(0x2C);
  LCD_WR_16BITS_DATA(color);	
}

void GUI_DrawPoint(uint16_t x, uint16_t y)
{	   
  LCD_SetWindow(x, y, x, y);			 	 
  LCD_WR_REG(0x2C);
  LCD_WR_16BITS_DATA(COLOR);	
}

void GUI_FillRect(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey)
{
  uint16_t i=0, j=0;
  LCD_SetWindow( sx, sy, ex-1, ey-1);
  LCD_WR_REG(0x2C);
  for(i=sx; i<ex; i++)
  {
    for(j=sy; j<ey; j++)
    {
      LCD_WR_16BITS_DATA(COLOR);
    }
  }
}
void GUI_ClearRect(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey)
{
  uint16_t i=0, j=0;
  LCD_SetWindow( sx, sy, ex-1, ey-1);
  LCD_WR_REG(0x2C);
  for(i=sx; i<ex; i++)
  {
    for(j=sy; j<ey; j++)
    {
      LCD_WR_16BITS_DATA(BKCOLOR);
    }
  }
}
void GUI_FillRectColor(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color)
{
  uint16_t i=0, j=0;
  LCD_SetWindow(sx, sy, ex-1, ey-1);
  LCD_WR_REG(0x2C);
  for(i=sx; i<ex; i++)
  {
    for(j=sy; j<ey; j++)
    {
      LCD_WR_16BITS_DATA(color);
    }
  }
}
void GUI_FillRectArry(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint8_t *arry)
{
  uint16_t i=0, j=0, color;
  LCD_SetWindow(sx, sy, ex-1, ey-1);
  LCD_WR_REG(0x2C);
  for(i=sx; i<ex; i++)
  {
    for(j=sy; j<ey; j++)
    {
      color = *arry;
      arry++;
      color = (color<<8) | (*arry);
      arry++;
      LCD_WR_16BITS_DATA(color);
    }
  }
}

//����
//x1,y1:�������
//x2,y2:�յ�����  
void GUI_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
  uint16_t t; 
  int xerr=0, yerr=0, delta_x, delta_y, distance; 
  int incx, incy, uRow, uCol; 
  delta_x = x2 - x1; //������������ 
  delta_y = y2 - y1; 
  uRow = x1; 
  uCol = y1; 
  if(delta_x > 0) 
    incx = 1; //���õ������� 
  else if(delta_x == 0) 
    incx = 0;//��ֱ�� 
  else 
  { incx = -1; delta_x = -delta_x;} 

  if(delta_y > 0)
    incy = 1; 
  else if(delta_y == 0) 
    incy = 0;//ˮƽ�� 
  else
  { incy = -1; delta_y = -delta_y;} 

  if(delta_x > delta_y)
    distance = delta_x; //ѡȡ��������������
  else
    distance = delta_y;

  for(t=0; t <= distance+1; t++ )//�������
  {
    GUI_DrawPoint(uRow,uCol);//����
    xerr += delta_x;
    yerr += delta_y;
    if(xerr > distance) 
    {
      xerr -= distance;
      uRow += incx;
    }
    if(yerr > distance)
    {
      yerr -= distance;
      uCol += incy;
    }
  }
}

#include "math.h"
void GUI_DrawAngleLine(uint16_t x, uint16_t y, uint16_t r, int16_t angle)
{
  uint16_t ex,ey;
  float a = angle * 3.1415926f / 180;
  ex = x + cos(a) * r;
  ey = y - sin(a) * r;
  GUI_DrawLine(x, y, ex, ey);
}
void GUI_HLine(uint16_t x1, uint16_t y, uint16_t x2)
{
  uint16_t i=0;
  LCD_SetWindow(x1, y, x2-1, y);	
  LCD_WR_REG(0x2C);
  for(i=x1; i<x2; i++)
  {
    LCD_WR_16BITS_DATA(COLOR);	
  }
}
void GUI_VLine(uint16_t x, uint16_t y1, uint16_t y2)
{
  uint16_t i;
  for(i=y1; i<y2; i++)
  {
    GUI_DrawPoint(x, i);
  }
}


//������	  
//(x1,y1),(x2,y2):���εĶԽ�����
void GUI_DrawRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
  GUI_HLine(x1, y1, x2);
  GUI_HLine(x1, y2-1, x2);
  GUI_VLine(x1, y1, y2);
  GUI_VLine(x2-1, y1, y2);
}

//��ָ��λ�û�һ��ָ����С��Բ
//(x,y):���ĵ�
//r    :�뾶
void GUI_DrawCircle(uint16_t x0, uint16_t y0, uint16_t r)
{
  int16_t a = 0,
  b = r,
  di = 3 - (r << 1);             //�ж��¸���λ�õı�־

  while(a <= b)
  {
    GUI_DrawPoint(x0+a, y0-b);             //5
    GUI_DrawPoint(x0+b, y0-a);             //0           
    GUI_DrawPoint(x0+b, y0+a);             //4               
    GUI_DrawPoint(x0+a, y0+b);             //6 
    GUI_DrawPoint(x0-a, y0+b);             //1       
    GUI_DrawPoint(x0-b, y0+a);             
    GUI_DrawPoint(x0-a, y0-b);             //2             
    GUI_DrawPoint(x0-b, y0-a);             //7     	         
    a++;
    //ʹ��Bresenham�㷨��Բ     
    if(di<0)
      di += (a<<2) + 6;	  
    else
    {
      di += 10 + ((a-b)<<2);   
      b--;
    } 						    
  }
} 									  
void  GUI_FillCircle(uint16_t x0, uint16_t y0, uint16_t r)
{  
  int16_t  draw_x0, draw_y0;			// ��ͼ���������
  int16_t  draw_x1, draw_y1;	
  int16_t  draw_x2, draw_y2;	
  int16_t  draw_x3, draw_y3;	
  int16_t  draw_x4, draw_y4;	
  int16_t  draw_x5, draw_y5;	
  int16_t  draw_x6, draw_y6;	
  int16_t  draw_x7, draw_y7;	
  int16_t  fill_x0, fill_y0;			// �������ı�����ʹ�ô�ֱ�����
  int16_t  fill_x1;
  int16_t  xx, yy;					// ��Բ���Ʊ���

  int16_t  di;						// ���߱���

  /* �������� */
  if(0 == r) return;

  /* �����4�������(0��90��180��270��)��������ʾ */
  draw_x0 = draw_x1 = x0;
  draw_y0 = draw_y1 = y0 + r;
  if(draw_y0 < LCD_HEIGHT)
  {  
    GUI_DrawPoint(draw_x0, draw_y0);	// 90��
  }    

  draw_x2 = draw_x3 = x0;
  draw_y2 = draw_y3 = y0 - r;
  if(draw_y2 >= 0)
  {  
    GUI_DrawPoint(draw_x2, draw_y2);	// 270��
  }

  draw_x4 = draw_x6 = x0 + r;
  draw_y4 = draw_y6 = y0;
  if(draw_x4 < LCD_WIDTH) 
  {
    GUI_DrawPoint(draw_x4, draw_y4);	// 0��
    fill_x1 = draw_x4;
  }
  else
  {
    fill_x1 = LCD_WIDTH;
  }

  fill_y0 = y0;							// �������������ʼ��fill_x0
  fill_x0 = x0 - r;						// �����������������fill_y1
  if(fill_x0<0) 
  fill_x0 = 0;

  GUI_HLine(fill_x0, fill_y0, fill_x1);

  draw_x5 = draw_x7 = x0 - r;
  draw_y5 = draw_y7 = y0;
  if(draw_x5 >= 0) 
  {  
    GUI_DrawPoint(draw_x5, draw_y5);	// 180��
  }
  if(1==r) return;

  /* ʹ��Bresenham�����л�Բ */
  di = 3 - 2*r;							// ��ʼ�����߱���
  xx = 0;
  yy = r;
  while(xx < yy)
  {
    if(di<0)
    {  
      di += 4*xx + 6;
    }
    else
    {  
      di += 4*(xx - yy) + 10;

      yy--;	  
      draw_y0--;
      draw_y1--;
      draw_y2++;
      draw_y3++;
      draw_x4--;
      draw_x5++;
      draw_x6--;
      draw_x7++;		 
    }

    xx++;   
    draw_x0++;
    draw_x1--;
    draw_x2++;
    draw_x3--;
    draw_y4++;
    draw_y5++;
    draw_y6--;
    draw_y7--;

    /* Ҫ�жϵ�ǰ���Ƿ�����Ч��Χ�� */
    if((draw_x0 <= LCD_WIDTH) && (draw_y0 >= 0))	
    {  
      GUI_DrawPoint(draw_x0, draw_y0);
    }	    
    if((draw_x1 >= 0) && (draw_y1 >= 0))	
    {  
      GUI_DrawPoint(draw_x1, draw_y1);
    }

    /* �ڶ���ˮֱ�����(�°�Բ�ĵ�) */
    if(draw_x1 >= 0)
    {  /* �������������ʼ��fill_x0 */
      fill_x0 = draw_x1;
      /* �������������ʼ��fill_y0 */
      fill_y0 = draw_y1;
      if(fill_y0 > LCD_HEIGHT) 
        fill_y0 = LCD_HEIGHT;
      if(fill_y0 < 0) 
        fill_y0 = 0; 
      /* �����������������fill_x1 */									
      fill_x1 = x0*2 - draw_x1;				
      if(fill_x1 > LCD_WIDTH) 
        fill_x1 = LCD_WIDTH;
      GUI_HLine(fill_x0, fill_y0, fill_x1);
    }

    if((draw_x2 <= LCD_WIDTH) && (draw_y2 <= LCD_HEIGHT) )	
    {  
      GUI_DrawPoint(draw_x2, draw_y2);   
    }

    if((draw_x3 >= 0) && (draw_y3 <= LCD_HEIGHT) )	
    {  
      GUI_DrawPoint(draw_x3, draw_y3);
    }

    /* ���ĵ㴹ֱ�����(�ϰ�Բ�ĵ�) */
    if(draw_x3 >= 0)
    {  /* �������������ʼ��fill_x0 */
      fill_x0 = draw_x3;
      /* �������������ʼ��fill_y0 */
      fill_y0 = draw_y3;
      if(fill_y0 > LCD_HEIGHT) 
        fill_y0 = LCD_HEIGHT;
      if(fill_y0 < 0) 
        fill_y0 = 0;
      /* �����������������fill_x1 */									
      fill_x1 = x0*2 - draw_x3;				
      if(fill_x1 > LCD_WIDTH) 
        fill_x1 = LCD_WIDTH;
      GUI_HLine(fill_x0, fill_y0, fill_x1);
    }

    if((draw_x4 <= LCD_WIDTH) && (draw_y4 >= 0))	
    {  
      GUI_DrawPoint(draw_x4, draw_y4);
    }
    if((draw_x5 >= 0) && (draw_y5 >= 0))	
    {  
      GUI_DrawPoint(draw_x5, draw_y5);
    }

    /* �����㴹ֱ�����(�ϰ�Բ�ĵ�) */
    if(draw_x5 >= 0)
    {  /* �������������ʼ��fill_x0 */
      fill_x0 = draw_x5;
      /* �������������ʼ��fill_y0 */
      fill_y0 = draw_y5;
      if(fill_y0 > LCD_HEIGHT) 
        fill_y0 = LCD_HEIGHT;
      if(fill_y0 < 0) 
        fill_y0 = 0;
      /* �����������������fill_x1 */									
      fill_x1 = x0*2 - draw_x5;				
      if(fill_x1 > LCD_WIDTH) 
        fill_x1 = LCD_WIDTH;
      GUI_HLine(fill_x0, fill_y0, fill_x1);
    }

    if((draw_x6 <= LCD_WIDTH) && (draw_y6 <= LCD_HEIGHT))
    {  
      GUI_DrawPoint(draw_x6, draw_y6);
    }

    if((draw_x7 >= 0) && (draw_y7 <= LCD_HEIGHT))	
    {  
      GUI_DrawPoint(draw_x7, draw_y7);
    }

    /* �ڰ˵㴹ֱ�����(�ϰ�Բ�ĵ�) */
    if(draw_x7 >= 0)
    {  /* �������������ʼ��fill_x0 */
      fill_x0 = draw_x7;
      /* �������������ʼ��fill_y0 */
      fill_y0 = draw_y7;
      if(fill_y0 > LCD_HEIGHT) 
        fill_y0 = LCD_HEIGHT;
      if(fill_y0 < 0) 
        fill_y0 = 0;
      /* �����������������fill_x1 */									
      fill_x1 = x0*2 - draw_x7;				
      if(fill_x1 > LCD_WIDTH) 
        fill_x1 = LCD_WIDTH;
      GUI_HLine(fill_x0, fill_y0, fill_x1);
    }
  }
}

//ɨ�跽ʽ: ���ϵ��¡�������
//0xA7A1-0xA7C1  0xA7D1-0xA7F1 66�������ַ�
//0xA140-0xA18C  76�����������ַ�
void GUI_DispChar(int16_t sx, int16_t sy, const uint16_t ch, uint8_t mode)
{  		
  uint8_t   x = 0, 
            y = 0, 
            j = 0, 
            i = 0;
  uint8_t   font[(BYTE_HEIGHT * BYTE_WIDTH / 8)];
  uint32_t  offset = 0;
  uint32_t  temp = 0;

  if(ch < 0xFF)  //ASCII�ַ�
  {
    offset = ch * (BYTE_HEIGHT * BYTE_WIDTH / 8);	
  }
  else if(ch >= 0xA7A1 && ch <= 0xA7F1)  //�����ַ�
  {
    offset = (ch-0xA7A1+127) * (BYTE_HEIGHT * BYTE_WIDTH / 8);
  }
  else if(ch >= 0xA140 && ch <= 0xA18C)  //���������ַ�
  {
    offset = (ch-0xA140+208) * (BYTE_HEIGHT * BYTE_WIDTH / 8);
  }	  
  W25Qxx_ReadBuffer(font, offset+BYTE_ADDR, (BYTE_HEIGHT * BYTE_WIDTH / 8));
  for(x=0; x < BYTE_WIDTH; x++)
  {   
    for(j=0; j < (BYTE_HEIGHT + 8-1)/8; j++)
    {
      temp <<= 8;
      temp |= font[i++];
    }

    for(y=0;y < BYTE_HEIGHT;y++)
    {			    
      if(temp & (1<<(BYTE_HEIGHT-1)))
      GUI_DrawPixel(sx,sy+y,COLOR);
      else if(mode!=0)
      GUI_DrawPixel(sx,sy+y,BKCOLOR);
      temp<<=1;
    } 
    sx++;
  }
}

void GUI_DispHz(int16_t sx, int16_t sy, const uint8_t *hz, uint8_t mode)
{
  uint8_t     x = 0, 
              y = 0, 
              j = 0, 
              i = 0;
  uint8_t     font[(BYTE_HEIGHT * BYTE_WIDTH*2 / 8)];
  uint32_t    offset = 0;					//GBK Encode
  uint32_t    temp = 0;

  if(hz[1]<0x7F) 
    offset=(190*(hz[0]-0x81)+hz[1]-0x40) * (BYTE_HEIGHT * BYTE_WIDTH*2 / 8);
  else 
    offset=(190*(hz[0]-0x81)+hz[1]-0x41) * (BYTE_HEIGHT * BYTE_WIDTH*2 / 8);

  W25Qxx_ReadBuffer(font, offset+WORD_ADDR, (BYTE_HEIGHT * BYTE_WIDTH*2 / 8));

  for(x=0; x < BYTE_WIDTH*2; x++)
  {           
    for(j=0; j < (BYTE_HEIGHT + 8-1)/8; j++)
    {
      temp <<= 8;
      temp |= font[i++];
    }

    for(y=0;y < BYTE_HEIGHT;y++)
    {			    
      if(temp & (1<<(BYTE_HEIGHT-1)))
        GUI_DrawPixel(sx,sy+y,COLOR);
      else if(mode!=0)
        GUI_DrawPixel(sx,sy+y,BKCOLOR);
      temp<<=1;
    } 
    sx++;
  }  	  
}

//����ֵ��4λ��p��ƫ������*p<0x81 ʱ���ǵ��ֽڱ��룬pÿ�ε�ƫ����Ϊ1. 
//                                    ������˫�ֽڱ��룬pÿ��ƫ����Ϊ2��
//      ��4λ��x����ƫ������ASCII�ַ��������ַ������������ַ� �������Ϊ BYTE_WIDTH
//                          �������ġ����� �������Ϊ 2��BYTE_WIDTH
uint8_t GUI_DispOne(int16_t x, int16_t y, const uint8_t *p, uint8_t mode)
{
  if(p==NULL || *p==0) return 0;

  if(*p<0x81)
  {			
    GUI_DispChar(x,y,*p,mode);
    return ((1<<4) | (1<<0));
  }
  else
  {
    if(isRussia(p) || isArmenian(p))
    {
      GUI_DispChar(x, y,(p[0]<<8)|p[1],mode);
      return ((2<<4) | (1<<0));
    }
    else
    {
      GUI_DispHz(x,y,p,mode);
      return ((2<<4) | (2<<0));
    }
  }
}    
const uint8_t* GUI_DispString(int16_t x, int16_t y, const uint8_t *p, uint8_t mode)
{     
  uint8_t res = 0;    
  while(1)
  {   
    res= GUI_DispOne(x, y, p, mode);
    if(res == 0) return p;
    x += (res&0x0F) * BYTE_WIDTH; //res����λ��x����ƫ��
    p += (res&0xF0)>>4;           //res����λ��pƫ����
  }
}

const uint8_t* GUI_DispLenString(int16_t x, int16_t y, const uint8_t *p, uint8_t mode, uint8_t len)
{       
  uint8_t res = 0, nlen = 0;
  while(nlen < len)
  {   
    res= GUI_DispOne(x, y, p, mode);
    if(res == 0) return p;
    x += (res&0x0F) * BYTE_WIDTH; //res����λ��x����ƫ��
    nlen += (res&0x0F);
    p += (res&0xF0)>>4;           //res����λ��pƫ����
  }
  return p;
}

void GUI_DispStringRight(int16_t x, int16_t y, const uint8_t *p, uint8_t mode)
{
  x -= my_strlen(p) * BYTE_WIDTH; 
  GUI_DispString(x, y, p, mode);
}

void GUI_DispStringInRect(int16_t sx, int16_t sy, int16_t ex, int16_t ey, const uint8_t *p, uint8_t mode)
{    
  uint16_t stringlen = my_strlen(p)*BYTE_WIDTH;
  uint16_t width = ex - sx;
  uint16_t height = ey - sy;
  uint8_t  nline = (stringlen+width-1)/width ;

  if(nline > height/BYTE_HEIGHT)
  nline = height/BYTE_HEIGHT;

  uint16_t x_offset = stringlen >= width ? 0 : ( width-stringlen)>>1;
  uint16_t y_offset = (nline*BYTE_HEIGHT) >= height ? 0 : (( height - (nline*BYTE_HEIGHT) )>>1);
  uint16_t x = sx + x_offset, y = sy + y_offset;

  uint8_t i=0;
  for(i=0; i<nline; i++)
  {
    p = GUI_DispLenString(x,y,p,mode,width/BYTE_WIDTH);
    y += BYTE_HEIGHT;
  }   
}

void GUI_DispStringInPrect(const GUI_RECT *rect, const uint8_t *p, uint8_t mode)
{    
  GUI_DispStringInRect(rect->x0, rect->y0, rect->x1, rect->y1,p,mode);
}

void GUI_DispCharInPrect(const GUI_RECT *rect, uint16_t p, uint8_t mode)
{    
  uint16_t x_offset = (rect->x1 - rect->x0 - BYTE_WIDTH)>>1;
  uint16_t y_offset = (rect->y1 - rect->y0 - BYTE_HEIGHT)>>1;
  GUI_DispChar(rect->x0+x_offset,rect->y0+y_offset,p,mode);	
}


const uint32_t GUI_Pow10[10] = {
1 , 10, 100, 1000, 10000,
100000, 1000000, 10000000, 100000000, 1000000000
};

void GUI_DispDec(int16_t x, int16_t y,int32_t num, uint8_t len, uint8_t mode, uint8_t leftOrRight)
{         	
  uint8_t i;
  uint8_t bit_value;
  uint8_t blank_bit_len = 0;
  uint8_t notZero = 0;	
  char    isNegative = 0;

  if(num<0)
  {
    num = -num;
    isNegative = 1;
  }        
  for(i=0;i<len;i++)
  {
    bit_value=(num/GUI_Pow10[len-i-1])%10;
    if(notZero == 0)
    {
      if(bit_value == 0 && i<(len-1))
      {
        if(leftOrRight==RIGHT)
        {
          GUI_DispChar(x,y,' ',mode);
          x += BYTE_WIDTH;
        }
        else
        {
          blank_bit_len++;
        }
        continue;
      }
      else 
      {
        notZero = 1; 
        if(isNegative)
        {                
          GUI_DispChar(x-BYTE_WIDTH,y,'-',mode);
        }
      }
    }
    GUI_DispChar(x,y,bit_value+'0',mode); 
    x += BYTE_WIDTH;
  }
  for(; blank_bit_len>0; blank_bit_len--)
  {
    GUI_DispChar(x,y,' ',mode); 
    x += BYTE_WIDTH;
  }
} 

void GUI_DispFloat(int16_t x, int16_t y, float num, uint8_t llen, uint8_t rlen, uint8_t mode, uint8_t leftOrRight)
{    
  uint8_t  alen = 0;
  uint8_t  i=0;
  uint8_t  notZero = 0;
  char     isNegative = 0;

  if(num<0)
  {
    num = -num;
    isNegative = 1;
  }        

  num *= GUI_Pow10[(unsigned)rlen];
  num += 0.5f;
  num = (float) floor(num);
  for(i=0; i<llen; i++)
  {
    uint8_t bit_value = ((uint32_t)(num/GUI_Pow10[llen+rlen-1-i]))%10;
    if(notZero == 0) 
    {
      if(bit_value == 0 && i<(llen-1))
      {
        if(leftOrRight==RIGHT)
        {
          GUI_DispChar(x,y,' ',mode);
          x += BYTE_WIDTH;
          alen++;
        }
        continue;
      }
      else 
      {
        notZero = 1; 
        if(isNegative)
        {                
          GUI_DispChar(x-BYTE_WIDTH,y,'-',mode);
        }
      }
    }
    GUI_DispChar(x,y,bit_value+'0',mode);
    x += BYTE_WIDTH;
    alen++;
  }
  GUI_DispChar(x,y,'.',mode);
  x += BYTE_WIDTH;
  alen++;

  for(i=0;i<rlen;i++)
  {
    GUI_DispChar(x,y,(int)(num/GUI_Pow10[rlen-1-i])%10+'0',mode);
    x += BYTE_WIDTH;
    alen++;
  }
  for(; alen < llen+rlen; alen++)
  {        
    GUI_DispChar(x,y,' ',mode);
    x += BYTE_WIDTH;
  }
} 

/****************************************************     Widget    *******************************************************************/
void RADIO_Create(RADIO *raido)
{
  uint8_t i=0;
  for(i=0;i<raido->num;i++)
  {
    if(i==raido->select)
      GUI_SetColor(ST7920_FNCOLOR);
    else
      GUI_SetColor(WHITE);
    GUI_FillCircle(raido->sx+BYTE_HEIGHT/2, i*raido->distance+raido->sy+BYTE_HEIGHT/2, BYTE_HEIGHT/8);
    GUI_DrawCircle(raido->sx+BYTE_HEIGHT/2, i*raido->distance+raido->sy+BYTE_HEIGHT/2, BYTE_HEIGHT/4);
    GUI_DispString(raido->sx+BYTE_HEIGHT,   i*raido->distance+raido->sy, raido->context[i], 1);
  }	
  GUI_SetColor(WHITE);
}

void RADIO_Select(RADIO *raido, uint8_t select)
{
  uint8_t i=0;
  if(raido->select==select)
  return;
  for(i=0;i<2;i++)
  {
    if(i==0)
    {
      GUI_SetColor(WHITE);
    }
    else
    {
      raido->select=select;
      GUI_SetColor(ST7920_FNCOLOR);
    }
    GUI_FillCircle(raido->sx+BYTE_HEIGHT/2, raido->select*raido->distance+raido->sy+BYTE_HEIGHT/2, BYTE_HEIGHT/8);
    GUI_DrawCircle(raido->sx+BYTE_HEIGHT/2, raido->select*raido->distance+raido->sy+BYTE_HEIGHT/2, BYTE_HEIGHT/4);
    GUI_DispString(raido->sx+BYTE_HEIGHT,   raido->select*raido->distance+raido->sy, raido->context[raido->select], 1);
  }
  GUI_SetColor(WHITE);
}

//������ʾ�ַ���
void Scroll_CreatePara(SCROLL * para, uint8_t *pstr ,GUI_RECT *rect)
{
  memset(para,0,sizeof(SCROLL));	
  para->text = pstr;
  para->len_size = strlen((char *)pstr);    
  para->len_total = my_strlen(pstr);
  para->len_max = ((rect->x1 - rect->x0)/BYTE_WIDTH);
  para->rect = rect;
}

int min(int value1,int value2)
{
  if(value1>value2)
  return value2;
  return value1;  
}

int max(int value1,int value2)
{
  if(value1>value2)
  return value1;
  return value2;  
}

void Scroll_DispString(SCROLL * para,uint8_t mode,uint8_t align)
{
  uint16_t i=0;		

  if(para->text == NULL) return;
  if(para->len_total > para->len_max)
  {
    if(OS_GetTime()-para->time>5)
    {
      para->time=OS_GetTime();			      
      GUI_SetRange(para->rect->x0, para->rect->y0, para->rect->x1, para->rect->y1);
      if(para->index < para->len_size)
      {
        para->off_head++;
        if((uint8_t)para->text[para->index]<0x81)
        {
          if(para->off_head==BYTE_WIDTH) {para->index++; para->off_head=0;}
        }
        else if( isRussia(&para->text[para->index]) || isArmenian(&para->text[para->index]) )
        {
          if(para->off_head==BYTE_WIDTH) {para->index += 2; para->off_head=0;}      
        }
        else
        {
          if(para->off_head==BYTE_WIDTH*2) {para->index += 2; para->off_head=0;}      
        }

        GUI_DispLenString(para->rect->x0 - para->off_head, para->rect->y0 ,&para->text[para->index],mode,para->len_max+4);

        for(i=para->rect->y0;i<para->rect->y1;i++)					//�����β
        {
          GUI_DrawPixel(para->rect->x0 - para->off_head + (min(para->len_size - para->index, para->len_max + 4) * BYTE_WIDTH), i, BKCOLOR);
        }
      }      
      if(para->len_size - para->index < para->len_max-4)  //��β����ĸ��ַ��ļ��
      {
        para->off_tail++;
        GUI_DispLenString(para->rect->x1-para->off_tail,para->rect->y0,para->text,mode,(para->off_tail + BYTE_WIDTH - 1)/BYTE_WIDTH);
        if(para->off_tail >= para->rect->x1-para->rect->x0)
        {
          para->off_head=0;
          para->off_tail=0;
          para->index=0;
        }
      }		
      GUI_CancelRange();
    }
  }
  else if(!para->has_disp)
  {
    switch(align)
    {
      case LEFT: 
      {
        GUI_DispString(para->rect->x0,para->rect->y0,para->text,mode);
        break;
      }
      case RIGHT: 
      {
        uint16_t x_offset=(para->rect->x1 - (para->len_total * BYTE_WIDTH));	
        GUI_DispString(x_offset,para->rect->y0,para->text,mode);
        break;
      }
      case CENTER:
      {
        uint16_t x_offset=((para->rect->x1 - para->rect->x0 - (para->len_total * BYTE_WIDTH)) >>1);	
        GUI_DispString(para->rect->x0+x_offset,para->rect->y0,para->text,mode);
        break;
      }
    }
    para->has_disp = 1;
  }
}


//���ư���
void GUI_DrawButton(const BUTTON *button, uint8_t pressed)
{
  const uint16_t radius = button->radius;
  const uint16_t lineWidth = button->lineWidth;
  const int16_t sx = button->rect.x0,
  sy = button->rect.y0,
  ex = button->rect.x1,
  ey = button->rect.y1;
  const uint16_t nowBackColor = GUI_GetBkColor();
  const uint16_t nowFontColor = GUI_GetColor();

  const uint16_t lineColor = pressed ? button->pLineColor : button->lineColor;
  const uint16_t backColor = pressed ? button->pBackColor : button->backColor;
  const uint16_t fontColor = pressed ? button->pFontColor : button->fontColor;

  GUI_SetColor(lineColor);
  GUI_FillCircle(sx + radius,     sy + radius,  radius);   //�ĸ��ǵ�Բ��
  GUI_FillCircle(ex - radius - 1, sy + radius,  radius);
  GUI_FillCircle(sx + radius,     ey - radius - 1, radius);
  GUI_FillCircle(ex - radius - 1, ey - radius - 1, radius);

  for(uint16_t i=0; i<lineWidth ;i++)
  {
    GUI_HLine(sx + radius, sy + i,      ex - radius);  //�ĸ����
    GUI_HLine(sx + radius, ey - 1 - i,  ex - radius);
    GUI_VLine(sx + i,      sy + radius, ey - radius);
    GUI_VLine(ex - 1 - i,  sy + radius, ey - radius);
  }
  GUI_SetColor(backColor);
  GUI_FillCircle(sx + radius,     sy + radius,  radius - lineWidth);   //����ĸ��ǵ�Բ��
  GUI_FillCircle(ex - radius - 1, sy + radius,  radius - lineWidth);
  GUI_FillCircle(sx + radius,     ey - radius - 1, radius - lineWidth);
  GUI_FillCircle(ex - radius - 1, ey - radius - 1, radius - lineWidth);
  GUI_FillRect(sx + radius, sy + lineWidth, ex - radius, sy + lineWidth + radius);
  GUI_FillRect(sx + lineWidth, sy + lineWidth + radius, ex - lineWidth, ey - lineWidth - radius);
  GUI_FillRect(sx + radius, ey - lineWidth - radius, ex - radius, ey - lineWidth);

  GUI_SetColor(fontColor);
  GUI_DispStringInPrect(&button->rect, button->context, 0);

  GUI_SetBkColor(nowBackColor);
  GUI_SetColor(nowFontColor);
}


void GUI_DrawWindow(const WINDOW *window, const uint8_t *title, const uint8_t *inf)
{
  const uint16_t titleHeight = window->title.height;
  const uint16_t infoHeight = window->info.height;
  const uint16_t radius = window->radius;
  const uint16_t lineWidth = window->lineWidth;
  const uint16_t lineColor = window->lineColor;
  const uint16_t infoBackColor = window->info.backColor;
  const uint16_t bottomBackColor = window->bottom.backColor;
  const int16_t  sx = window->rect.x0,
  sy = window->rect.y0,
  ex = window->rect.x1,
  ey = window->rect.y1;
  const uint16_t nowBackColor = GUI_GetBkColor();
  const uint16_t nowFontColor = GUI_GetColor();

  GUI_SetColor(lineColor);
  GUI_FillCircle(sx + radius,      sy + radius,  radius);
  GUI_FillCircle(ex - radius - 1,  sy + radius,  radius);
  GUI_FillRect(sx + radius,  sy,         ex-radius, sy+radius);
  GUI_FillRect(sx,           sy+radius,  ex,        sy+titleHeight);
  for(uint16_t i=0; i<lineWidth ;i++)
  {
    GUI_VLine(sx + i,      sy + titleHeight, ey - radius);
    GUI_VLine(ex - 1 - i,  sy + titleHeight, ey - radius);
    GUI_HLine(sx + radius, ey - 1 - i,       ex - radius);
  }
  GUI_FillCircle(sx + radius,     ey - radius - 1, radius);
  GUI_FillCircle(ex - radius - 1, ey - radius - 1, radius);

  GUI_SetColor(infoBackColor);
  GUI_FillRect(sx + lineWidth, sy + titleHeight, ex - lineWidth, sy + titleHeight + infoHeight);
  GUI_SetColor(bottomBackColor);
  GUI_FillCircle(sx + radius,     ey - radius - 1, radius - lineWidth);
  GUI_FillCircle(ex - radius - 1, ey - radius - 1, radius - lineWidth);
  GUI_FillRect(sx + lineWidth,          sy + titleHeight + infoHeight, ex - lineWidth,          ey - lineWidth - radius);
  GUI_FillRect(sx + lineWidth + radius, ey - lineWidth - radius,       ex - lineWidth - radius, ey - lineWidth);

  GUI_SetColor(window->title.fontColor);
  //    GUI_DispStringInRect(rect.x0, rect.y0, rect.x1, rect.y0+titleHeight,title,0);
  GUI_DispString(sx+radius, sy+8, title,0);
  GUI_SetColor(window->info.fontColor);
  GUI_DispStringInRect(sx+lineWidth+BYTE_WIDTH, sy+titleHeight, ex-lineWidth-BYTE_WIDTH, sy+titleHeight+infoHeight, inf, 0); 

  GUI_SetBkColor(nowBackColor);
  GUI_SetColor(nowFontColor);
}
