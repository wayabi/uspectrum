#ifndef __WAVE_H_INCLUDED__
#define __WAVE_H_INCLUDED__

#define HEADERSIZE 44

typedef struct{
	signed short l;
	signed short r;
}Soundsample16;

typedef struct{
	unsigned char l;
	unsigned char r;
}Soundsample8;

typedef struct{
	unsigned short channelnum;				//$B%b%N%i%k$J$i(B1$B!"%9%F%l%*$J$i(B2
	unsigned long samplingrate;				//Hz$BC10L(B
	unsigned short bit_per_sample;		//1$B%5%s%W%k$"$?$j$N(Bbit$B?t(B
	unsigned long datanum;						//$B%b%N%i%k$J$i%5%s%W%k?t$r!"%9%F%l%*$J$i:81&#1%5%s%W%k$:$D$NAH$N?t(B

	unsigned char *monaural8;					//8$B%S%C%H%b%N%i%k$N%G!<%?$J$i$3$l$r;H$&(B
	signed short *monaural16;						//16$B%S%C%H%b%N%i%k$J$i$P$3$l$r;H$&(B
	Soundsample8 *stereo8;						//8$B%S%C%H%9%F%l%*$J$i$P$3$l$r;H$&(B
	Soundsample16 *stereo16;					//16$B%S%C%H%9%F%l%*$J$i$P$3$l$r;H$&(B
}Sound;

//$B<hF@$K@.8y$9$l$P%]%$%s%?$r<:GT$9$l$P(BNULL$B$rJV$9(B
Sound *Read_Wave(const char *filename);

//$B=q$-9~$_$K@.8y$9$l$P(B0$B$r<:GT$9$l$P(B1$B$rJV$9(B
int Write_Wave(const char *filename, Sound *snd);

//Sound$B$r:n@.$7!"0z?t$N>pJs$K9g$o$;$FNN0h$N3NJ]$r$9$k!#;H$o$l$k7A<00J30$NNN0h$N%]%$%s%?$O(BNULL
//$B@.8y$9$l$P%]%$%s%?$r!"<:GT$9$l$P(BNULL$B$rJV$9(B
Sound *Create_Sound(unsigned short channelnum, unsigned long samplingrate, unsigned short bit_per_sample, unsigned long datasize);

//Sound$B$r3+J|$9$k(B
void Free_Sound(Sound *snd);

#endif /*__WAVE_H_INCLUDED__*/


