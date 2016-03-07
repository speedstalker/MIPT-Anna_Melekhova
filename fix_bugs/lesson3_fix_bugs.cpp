/**
* @brief
*		Find errors and decrease probability of getting errors of the same kind in the future
*		This piece of code won't compile and it doesn't describe an entire algorithm: just part of some page storage
*
* @author
*		AnnaM
*/

#include <Windows.h>
#include <stdio.h>
// ERROR: maybe some additional libraries are required

enum PAGE_COLOR
{
        // ERROR: begin with 0, add numb_of_elems
	PG_COLOR_GREEN = 1, /* page may be released without high overhead */
	PG_COLOR_YELLOW, /* nice to have */
	PG_COLOR_RED	/* page is actively used */
};


/**
 * UINT Key of a page in hash-table (prepared from color and address)
 */
union PageKey
{
        // ERROR: CHAR -> UINT
        // ERROR: 24 -> SMTH (CHAR_BIT) in case of x64
	struct
	{
        CHAR	cColor: 8;
	UINT	cAddr: 24;
	};

	UINT	uKey;
};


/* Prepare from 2 chars the key of the same configuration as in PageKey */
// ERROR: parenthesis around <<
// ERROR: maybe cast ptr to smth
#define CALC_PAGE_KEY( Addr, Color )	(  (Color) + (Addr) << 8 )


/**
 * Descriptor of a single guest physical page
 */
struct PageDesc
{
// ERROR: 'union' and 'struct' needs to be added || maybe not, as this is C++
// ERROR: uKey has a member uKey
	PageKey			uKey;

	/* list support */
	PageDesc		*next, *prev;
};

// ERROR: do while wrapping
// ERROR: should be uKey.uKey
#define PAGE_INIT( Desc, Addr, Color )              \
    {                                               \
        (Desc).uKey = CALC_PAGE_KEY( Addr, Color ); \
        (Desc).next = (Desc).prev = NULL;           \
    }


/* storage for pages of all colors */
// ERROR: 3 -> total_numb in enum
// ERROR: add 'struct'
static PageDesc* PageStrg[ 3 ];

// ERROR: do we actually need this function?
void PageStrgInit()
{
// ERROR: additional library for memset required
// ERROR: no '&' required
	memset( PageStrg, 0, sizeof(&PageStrg) );
}

// ERROR: struct
// ERROR: color not 'char' but 'enum'
PageDesc* PageFind( void* ptr, char color )
// ERROR: check for color boundaries
{
        // ERROR: struct
	for( PageDesc* Pg = PageStrg[color]; Pg; Pg = Pg->next );
        // ERROR: Pg always == NULL after this 'for' || no, just delete ';' after 'for'
        // ERROR: uKey.uKey
        // ERROR: CALC_PAGE_KEY returns x86 value, correct the macro
        if( Pg->uKey == CALC_PAGE_KEY(ptr,color) )
           return Pg;
    return NULL;
}

// ERROR: struct
PageDesc* PageReclaim( UINT cnt )
{
        // ERROR: not 'UINT' but 'enum'
	UINT color = 0;
        // ERROR: init
	PageDesc* Pg;
	while( cnt )
	{
		Pg = Pg->next;
                // ERROR: remove (Pg) ?
                // ERROR: first remove, then ->next
		PageRemove( PageStrg[ color ] );
		cnt--;
		if( Pg == NULL )
		{
                        // ERROR: check boundaries
			color++;
			Pg = PageStrg[ color ];
		}
	}
        // ERROR: return statement
}

// ERROR: not UINT but enum
PageDesc* PageInit( void* ptr, UINT color )
// ERROR: check for color boundaries
{
    // ERROR: additional library for 'new'
    PageDesc* pg = new PageDesc;
    // ERROR: throws exception
    if( pg )
        // ERROR: no & needed
        PAGE_INIT(&pg, ptr, color);
    else
        printf("Allocation has failed\n");
    return pg;
}

/**
 * Print all mapped pages
 */
void PageDump()
{
        // ERROR: not UINT but enum
	UINT color = 0;
	#define PG_COLOR_NAME(clr) #clr
	char* PgColorName[] =
	{
                // ERROR: inverted order
		PG_COLOR_NAME(PG_COLOR_RED),
		PG_COLOR_NAME(PG_COLOR_YELLOW),
		PG_COLOR_NAME(PG_COLOR_GREEN)
	};

	while( color <= PG_COLOR_RED )
	{
                // ERROR: wrong order of 'printf' arguments
		printf("PgStrg[(%s) %u] ********** \n", color, PgColorName[color] );
                // ERROR: prefix incrementation
		for( PageDesc* Pg = PageStrg[++color]; Pg != NULL; Pg = Pg->next )
		{
                        // ERROR: ==
                        // ERROR: uKey.uAddr
                        // ERROR: uAddr -> cAddr
			if( Pg->uAddr = NULL )
				continue;

                        // ERROR: uKey.uKey & cAddr
                        // ERROR: maybe (void*)
			printf("Pg :Key = 0x%x, addr %p\n", Pg->uKey, Pg->uAddr );
		}
	}
	#undef PG_COLOR_NAME
}
