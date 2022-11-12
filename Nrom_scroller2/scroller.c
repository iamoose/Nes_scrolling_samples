/*	simple Hello World, for cc65, for NES
 *  writing to the screen with rendering ON, via a vram buffer
 *	using neslib
 *	Doug Fraker 2018
 */	
 

// using a buffer, we have more flexibility, since we can adjust the 
// screen position of a PPU write, and can piggy back multiple data 
// sets into 1 push, doing more than 1 update per frame, and 
// the data sets can have zeroes, since they are not zero terminated

#include "LIB/neslib.h"
#include "LIB/nesdoug.h"


#include "bgsplit_nameh.h"	//packed nametable data


#define BLACK 0x0f
#define DK_GY 0x00
#define LT_GY 0x10
#define WHITE 0x30
// there's some oddities in the palette code, black must be 0x0f, white must be 0x30


const unsigned char palette[]={
BLACK, DK_GY, LT_GY, WHITE,
0,0,0,0,
0,0,0,0,
0,0,0,0
}; 


const unsigned char palettx[16]={ 0x0f,0x1c,0x12,0x21,0x0f,0x01,0x21,0x32,0x0f,0x1c,0x12,0x21,0x0f,0x01,0x21,0x32 };






const unsigned char text[]={
0x64,
0x64,
0x64,
0x64,
0x64,
0x64,
0x64,
0x64,
0x64,
0x64,
0x64,
0x64,
0x64,
0x64,
0x64,
0x64,
0x64,
0x64,
0x64,
0x64,
0x64,
0x64,
0x64,
0x64,
0x64,
0x64,
0x64,
0x64,
0x64,
0x64,
}; // not a c string, so no zero terminating


const unsigned char texty[]={
0x65,
0x65,
0x65,
0x65,
0x65,
0x65,
0x65,
0x65,
0x65,
0x65,
0x65,
0x65,
0x65,
0x65,
0x65,
0x65,
0x65,
0x65,
0x65,
0x65,
0x65,
0x65,
0x65,
0x65,
0x65,
0x65,
0x65,
0x65,
0x65,
0x65,
}; // not a c string, so no zero terminating



const unsigned char texto[]={
0x62,
0x62,
0x62,
0x62,
0x62,
0x62,
0x62,
0x62,
0x62,
0x62,
0x62,
0x62,
0x62,
0x62,
0x62,
0x62,
0x62,
0x62,
0x62,
0x62,
0x62,
0x62,
0x62,
0x62,
0x62,
0x62,
0x62,
0x62,
0x62,
0x62,
}; // not a c string, so no zero terminating


const unsigned char textm[]={
0x63,
0x63,
0x63,
0x63,
0x63,
0x63,
0x63,
0x63,
0x63,
0x63,
0x63,
0x63,
0x63,
0x63,
0x63,
0x63,
0x63,
0x63,
0x63,
0x63,
0x63,
0x63,
0x63,
0x63,
0x63,
0x63,
0x63,
0x63,
0x63,
0x63,
}; // not a c string, so no zero terminating




// black color 

const unsigned char textoh[]={
0x64,
0x64,
0x64,
0x64,
0x64,
0x64,
0x64,
0x64,
0x64,
0x64,
0x64,
0x64,
0x64,
0x64,
0x64,
0x64,
0x64,
0x64,
0x64,
0x64,
0x64,
0x64,
0x64,
0x64,
0x64,
0x64,
0x64,
0x64,
0x64,
0x64,
0x64,
0x64,
}; // not a c string, so no zero terminating


//color 2
const unsigned char textot[]={
0x65,
0x65,
0x65,
0x65,
0x65,
0x65,
0x65,
0x65,
0x65,
0x65,
0x65,
0x65,
0x65,
0x65,
0x65,
0x65,
0x65,
0x65,
0x65,
0x65,
0x65,
0x65,
0x65,
0x65,
0x65,
0x65,
0x65,
0x65,
0x65,
0x65,
0x65,
0x65,
}; // not a c string, so no zero terminating

//color 1
const unsigned char textop[]={
0x62,
0x62,
0x62,
0x62,
0x62,
0x62,
0x62,
0x62,
0x62,
0x62,
0x62,
0x62,
0x62,
0x62,
0x62,
0x62,
0x62,
0x62,
0x62,
0x62,
0x62,
0x62,
0x62,
0x62,
0x62,
0x62,
0x62,
0x62,
0x62,
0x62,
0x62,
0x62,
}; // not a c string, so no zero terminating


//color 3
const unsigned char textobb[]={
0x63,
0x63,
0x63,
0x63,
0x63,
0x63,
0x63,
0x63,
0x63,
0x63,
0x63,
0x63,
0x63,
0x63,
0x63,
0x63,
0x63,
0x63,
0x63,
0x63,
0x63,
0x63,
0x63,
0x63,
0x63,
0x63,
0x63,
0x63,
0x63,
0x63,
0x63,
0x63,
}; // not a c string, so no zero terminating







unsigned int tables;
unsigned int table2;
unsigned char pad1;

unsigned char dummyaddress;

unsigned int scroll_x;
static int scroll_y;
unsigned int scrolling_;


int address;







void main (void) {


	tables = TRUE;
	scrolling_ = 0;


	vram_adr(NAMETABLE_A);//unpack nametable into VRAM
	vram_unrle(bgsplit_nameh);


	

	ppu_on_all(); // turn on screen
	
	pal_bg(palettx); //	load the palette, this can be done any time, even with rendering on
					 // but, it's competing with the vram_buffer for v-blank time
					 // so, let's wait a v-blank first, so all the palette changes are done
	pal_spr(palettx);				
					 
					 
	ppu_wait_nmi(); // wait
	
	// now fill the vram_buffer
	
	set_vram_buffer(); // points ppu update to vram_buffer, do this at least once
						 
	
	scroll_x = 0;
	scroll_y = 0;
	
	
	while (1){
		// infinite loop
		// game code can go here later.
char positioner;



	
			if(table2 == 0)
		{
	if(scrolling_ == 1 && scroll_x % 8)
	{

		if(tables == 0)
		{
			
			address = get_ppu_addr(tables , scroll_x + 0xff, scroll_y  ); // (char nt, char x, char y);
			
			positioner = scroll_y / 8 ;
			
			multi_vram_buffer_vert(text, 30 - positioner  , address);
			
		
		
			if(positioner < 30 && scroll_y > 0)
			{
				address = get_ppu_addr(tables , scroll_x  + 0xff ,  0 );
				
				multi_vram_buffer_vert(texty, 1 +  positioner , address);
			}
			
		}
		
		if(tables == 1)
		{	

			address = get_ppu_addr(tables  , scroll_x + 0xff, scroll_y  ); 
	 
			positioner = scroll_y / 8 ;
	
			multi_vram_buffer_vert(texto, 30 -   positioner , address);
			
			
			
			if(positioner < 30 && scroll_y > 0)
			{
				address = get_ppu_addr(tables , scroll_x  + 0xff ,  0 );
				
				multi_vram_buffer_vert(textm, 1 +  positioner , address);
			}
			
			
		}

	}

	if(scrolling_ == 2 && scroll_x % 8)
	{

	
		if(tables == 0)
		{

		address = get_ppu_addr(tables + 1, scroll_x - 0xff , scroll_y );	
			
		positioner = (scroll_y ) / 8 ;
			
		multi_vram_buffer_vert(texto, 30 - positioner, address);
		
			if(positioner < 30 && scroll_y > 0)
			{
				address = get_ppu_addr(tables + 1, scroll_x  - 0xff ,  0 );
				
				multi_vram_buffer_vert(textm, 1 +  positioner , address);
			}
			
			
		}
		if(tables == 1)
		{
		

		address = get_ppu_addr(tables - 1, 510 + scroll_x ,scroll_y );
		
		
		positioner = (scroll_y ) / 8 ;
		
		
		multi_vram_buffer_vert(text, 30 - positioner, address);
		
		
		if(positioner < 30 && scroll_y > 0)
			{
				address = get_ppu_addr(tables - 1, 510 + scroll_x ,  0 );
				
				multi_vram_buffer_vert(texty,   positioner + 1 , address);
			}
		
		
		
		}
		

	}
		}
		
		
	if(table2 == 2)
		{
			
			
			
			
			if(scrolling_ == 1 && scroll_x % 8)
	{

		if(tables == 0)
		{
			
		address = get_ppu_addr(tables,scroll_x + 0xff,scroll_y - 248); // (char nt, char x, char y);
			
        positioner = (scroll_y -240) / 8 ;		
			
		multi_vram_buffer_vert(texty, 30 - positioner + 1, address);
		
		
		if(positioner < 30)
			{
				address = get_ppu_addr(tables , scroll_x + 0xff ,  0  );
				
				multi_vram_buffer_vert(text,  positioner - 1 , address);
			}
		
		
		}
		
		if(tables == 1)
		{	
	
		address = get_ppu_addr(tables,scroll_x + 0xff,scroll_y - 248); // (char nt, char x, char y);
	
		positioner = (scroll_y -240) / 8 ;	
	
		multi_vram_buffer_vert(textm, 30 - positioner + 1, address);
		
		if(positioner < 30)
			{
				address = get_ppu_addr(tables , scroll_x + 0xff ,  0  );
				
				multi_vram_buffer_vert(texto,  positioner - 1 , address);
			}
		
		
		
		}

	}



	if(scrolling_ == 2 && scroll_x % 8)
	{

	
	
	
		if(tables == 0)
		{

		address = get_ppu_addr(tables + 1, scroll_x - 0xff ,scroll_y - 248);	
			
		positioner = (scroll_y -240) / 8 ;	
			
		multi_vram_buffer_vert(textm,  1+ 30 - positioner, address);
		
			if(positioner < 30 && scroll_y > 0)
			{
			address = get_ppu_addr(tables + 1, scroll_x - 0xff ,0);	
			
			multi_vram_buffer_vert(texto,  positioner - 1 , address);
			}
			
		}
		if(tables == 1)
		{
		

		address = get_ppu_addr(tables - 1, 510 + scroll_x ,scroll_y - 248);
		
		positioner = (scroll_y -240) / 8 ;
		
		multi_vram_buffer_vert(texty, 1 + 30 - positioner, address);
		
			if(positioner < 30 && scroll_y > 0)
			{
			address = get_ppu_addr(tables - 1, 510 + scroll_x ,0);
			
			multi_vram_buffer_vert(text,  positioner - 1 , address);
			}
		
		}
		

	}
			
			
			
			
			
		}








	if(scrolling_ == 3 && scroll_y % 8)
	{
		
		
		
	if(tables == 1)
	{

		if(table2 == 0)
		{ 
			address = get_ppu_addr(table2 + 2,   scroll_x , scroll_y - 239 - 17);	
			
			positioner = 32 - scroll_x / 8;
			
			multi_vram_buffer_horz(textot, positioner, address);
			
			if(positioner < 32)
			{
				address = get_ppu_addr(tables,   0, scroll_y - 239 - 17);	
				
				multi_vram_buffer_horz(textobb, 1 + 32 - positioner, address); //nametable b partial load
			}
			
		}
		
		
		
		if(table2 == 2)
		{

			address = get_ppu_addr(table2 - 2,  scroll_x , scroll_y - 239 - 17);	
			
			positioner = 32 - scroll_x / 8;
			
			multi_vram_buffer_horz(textoh, 32 - scroll_x / 8, address);
			
			if(positioner < 32)
			{
				address = get_ppu_addr(tables ,   0, scroll_y - 239 - 17);
				
				multi_vram_buffer_horz(textop, 1+ 32 - positioner, address); //nametable b partial load
			}
			
			
		}
		
		
	}
	
	
	
	
	
	if(tables == 0)
	{

		if(table2 == 0)
		{

			address = get_ppu_addr(table2 + 1,  scroll_x , scroll_y - 239 - 17);	
			
			positioner = 32 - (scroll_x - 254)/ 8;
			
			multi_vram_buffer_horz(textobb, 32 - (scroll_x - 254)/ 8, address);
			
			
			if(positioner < 32)
			{
				address = get_ppu_addr(tables ,   0, scroll_y - 239 - 17);
				
				multi_vram_buffer_horz(textot, 1+ 32 - positioner, address); //nametable b partial load
			}
			
			
			
		}
		
		
		if(table2 == 2)
		{

			address = get_ppu_addr(table2 - 1,  scroll_x  , scroll_y - 239 - 17);	
			
			positioner = 32 - (scroll_x - 254)/ 8;
			
			multi_vram_buffer_horz(textop, 32 - (scroll_x - 254)/ 8, address);
			
			
			if(positioner < 32)
			{
				address = get_ppu_addr(tables ,   0, scroll_y - 239 - 17);
				
				multi_vram_buffer_horz(textoh, 1+ 32 - positioner, address); //nametable b partial load
			}
			
			
		}
		
		
	
		
	}
	
	
	}





	



	
	
	
	pad1 = pad_poll(0); 

	
	
	
	
	scrolling_ = 0;

	
	
	
	
	
	
	
	if (pad1 & PAD_RIGHT && scrolling_ == 0) 
	{
	
	scrolling_ = 1;
	
	
	if(scroll_x <= 510)
	{
	scroll_x += 1;
	}
	
	if(scroll_x > 510)
	{
		scroll_x = 0;
	}
	
	
	
	if(scroll_x <= 0xff)
	{
	tables = TRUE;
	}
	if(scroll_x > 0xff)
	{
	tables = FALSE;
	}

	set_scroll_x(scroll_x);
	
	}
	
	
	
	
	
	
	
	if (pad1 & PAD_LEFT  && scrolling_ == 0) 
	{
	
	
	scrolling_ = 2;
	
	
	
	if(scroll_x == 0)
	{
		scroll_x = 511;
	}
	
	if(scroll_x > 0)
	{
	scroll_x -= 1;
	}
	
	

	

	if(scroll_x <= 255)
	{
		tables = TRUE;
		//scroll_x = 254;
	}
	if(scroll_x > 255)
	{
		tables = FALSE;
		//scroll_x = 254;
	}
	
	
	set_scroll_x(scroll_x);
	
	}
	
	
	
	
	
	
	
	
		if (pad1 & PAD_DOWN  && scrolling_ == 0) 
		{
	
		scrolling_ = 3;
		
		if(scroll_y <= 479)
		{
		scroll_y = add_scroll_y(1, scroll_y);
		}
	
	
	
	
		if(scroll_y > 479)
		{
		scroll_y = -32;
		}
	
	
	
	
		if(scroll_y >= 239 + 18)
		{
			if(scroll_y <= 479 - 12)
			{
			table2 = 2;
			}
		}
		if(scroll_y > 0)// && scroll_y > 4)
		{
			if(scroll_y < 239 + 12)
			{
			table2 = 0;
			}
		}
	
	
	
	
	
		set_scroll_y(scroll_y);
	
		}
	
	
	
	
	
	
	
	
		//	if (pad1 & PAD_UP ) 
		//{
	
		//scrolling_ = 4;
		//scroll_y = sub_scroll_y(1, scroll_y);

	
	
		//if(scroll_y < - 32)
		//{
		//scroll_y = 479;
		//}
	
	
		//set_scroll_y(scroll_y);
	
		//}
	
	
	
	
	
	
	
	
	
	ppu_wait_nmi(); // waits till nmi, and push new updates to the ppu
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	

		
	}
}
	
	