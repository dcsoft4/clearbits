//  The authors have released ID3Lib as Public Domain (PD) and claim no copyright,
//  patent or other intellectual property protection in this work.  This means that
//  it may be modified, redistributed and used in commercial and non-commercial
//  software and hardware without restrictions.  ID3Lib is distributed on an "AS IS"
//  basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.
//  
//  The ID3Lib authors encourage improvements and optimisations to be sent to the
//  ID3Lib coordinator, currently Dirk Mahoney (dirk@id3.org).  Approved
//  submissions may be altered, and will be included and released under these terms.
//  
//  Mon Nov 23 18:34:01 1998
//
//  DC - FROM last public domain version of ID3Lib 3.05a

#ifndef	ID3LIB_TYPES_28BITINT_H
#define	ID3LIB_TYPES_28BITINT_H


//from "id3_types.h"
typedef long unsigned int	luint;
typedef	unsigned char		uchar;
typedef long signed int		lsint;


class int28
{
public:
				int28							( luint val = 0 );
				int28							( uchar *val );

uchar			operator[]						( luint posn );
luint			get								( void );

protected:
void			set								( luint val );
uchar			value[ sizeof ( luint ) ];		// the integer stored as a uchar array
};

#endif


