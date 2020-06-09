#include"wave.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

Sound *Read_Wave(const char *filename)
{
	unsigned int i;
	unsigned char header_buf[20];			        //$B%U%)!<%^%C%H%A%c%s%/$N%5%$%:$^$G$N%X%C%@>pJs$r<h$j9~$`(B
	FILE *fp;
	Sound *snd;
	unsigned long datasize;										//$BGH7A%G!<%?$N%P%$%H?t(B
	unsigned short fmtid;											//fmt$B$N(BID$B$r3JG<$9$k(B
	unsigned short channelnum;								//$B%A%c%s%M%k?t(B
	unsigned long samplingrate;								//$B%5%s%W%j%s%0<~GH?t(B
	unsigned short bit_per_sample;						//$BNL;R2=%S%C%H?t(B
	unsigned char *buf;												//$B%U%)!<%^%C%H%A%c%s%/(BID$B$+$i3HD%ItJ,$^$G$N%G!<%?$r<h$j9~$`(B
	unsigned long fmtsize;

	if((fp = fopen(filename, "rb")) == NULL){
		fprintf(stderr, "Error: %s could not read.", filename);
		return NULL;
	}

	fread(header_buf, sizeof(unsigned char), 20, fp);		//$B%U%)!<%^%C%H%A%c%s%/%5%$%:$^$G$N%X%C%@ItJ,$r<h$j9~$`(B

	//$B%U%!%$%k$,(BRIFF$B7A<0$G$"$k$+(B
	if(strncmp((char*)header_buf, "RIFF", 4)){
		fprintf(stderr, "Error: %s is not RIFF.", filename);
		fclose(fp);
		return NULL;
	}

	//$B%U%!%$%k$,(BWAVE$B%U%!%$%k$G$"$k$+(B
	if(strncmp((char*)(header_buf + 8), "WAVE", 4)){
		fprintf(stderr, "Error: %s is not WAVE.", filename);
		fclose(fp);
		return NULL;
	}

	//fmt $B$N%A%'%C%/(B
	if(strncmp((char*)(header_buf + 12), "fmt ", 4)){
		fprintf(stderr, "Error: %s fmt not found.", filename);
		fclose(fp);
		return NULL;
	}

	memcpy(&fmtsize, header_buf + 16, sizeof(fmtsize));

	if((buf = (unsigned char *)malloc(sizeof(unsigned char)*fmtsize)) == NULL){
		fprintf(stderr, "Allocation error\n");
		fclose(fp);
		return NULL;
	}

	fread(buf, sizeof(unsigned char), fmtsize, fp);		  //$B%U%)!<%^%C%H(BID$B$+$i3HD%ItJ,$^$G$N%X%C%@ItJ,$r<h$j9~$`(B

	memcpy(&fmtid, buf, sizeof(fmtid));			//LinearPCM$B%U%!%$%k$J$i$P(B1$B$,F~$k(B

	if(fmtid!=1){
		fprintf(stderr, "Error: %s is not LinearPCM.", filename);
		fclose(fp);
		return NULL;
	}

	memcpy(&channelnum, buf + 2, sizeof(channelnum));	//$B%A%c%s%M%k?t$r<hF@(B
	memcpy(&samplingrate, buf + 4, sizeof(samplingrate)); //$B%5%s%W%j%s%0<~GH?t$r<hF@(B
	memcpy(&bit_per_sample, buf + 14, sizeof(bit_per_sample)); //$BNL;R2=%S%C%H?t$r<hF@(B

	fread(buf, sizeof(unsigned char), 8, fp);		  //fact$B$b$7$/$O(Bdata$B$N(BID$B$H%5%$%:$r<hF@(B8$B%P%$%H(B
	
	if(!strncmp((char*)buf, "fact", 4)){
		fread(buf, sizeof(unsigned char), 4, fp);
		fread(buf, sizeof(unsigned char), 8, fp);
	}

	if(strncmp((char*)buf, "data", 4)){
		fprintf(stderr, "Error: %s data part not found.", filename);
		fclose(fp);
		return NULL;
	}

	memcpy(&datasize, buf + 4, sizeof(datasize)); //$BGH7A%G!<%?$N%5%$%:$N<hF@(B

	if((snd = Create_Sound(channelnum, samplingrate, bit_per_sample, datasize)) == NULL){
		fclose(fp);
		return NULL;
	}

	if(channelnum==1 && bit_per_sample==8){
		fread(snd->monaural8, sizeof(unsigned char), snd->datanum, fp);		//$B%G!<%?ItJ,$rA4$F<h$j9~$`(B
	}else if(channelnum==1 && bit_per_sample==16){
		fread(snd->monaural16, sizeof(signed short), snd->datanum, fp);
	}else if(channelnum==2 && bit_per_sample==8){
		for(i=0; i<snd->datanum; i++){
			fread(&(snd->stereo8[i].l), sizeof(unsigned char), 1, fp);
			fread(&(snd->stereo8[i].r), sizeof(unsigned char), 1, fp);
		}
	}else if(channelnum==2 && bit_per_sample==16){
		for(i=0; i<snd->datanum; i++){
			fread(&(snd->stereo16[i].l), sizeof(signed short), 1, fp);
			fread(&(snd->stereo16[i].r), sizeof(signed short), 1, fp);
		}
	}else{
		fprintf(stderr, "Header is destroyed.");
		fclose(fp);
		Free_Sound(snd);
	}

	return snd;
}

int Write_Wave(const char *filename, Sound *snd)
{
	int i;
	FILE *fp;
	unsigned char header_buf[HEADERSIZE]; //$B%X%C%@$r3JG<$9$k(B
	unsigned long fswrh;  //$B%j%U%X%C%@0J30$N%U%!%$%k%5%$%:(B
	unsigned long fmtchunksize; //fmt$B%A%c%s%/$N%5%$%:(B
	unsigned long dataspeed;		//$B%G!<%?B.EY(B
	unsigned short blocksize;   //1$B%V%m%C%/$"$?$j$N%P%$%H?t(B
	unsigned long datasize;			//$B<~GH?t%G!<%?$N%P%$%H?t(B
	unsigned short fmtid;				//$B%U%)!<%^%C%H(BID

	if((fp = fopen(filename, "wb")) == NULL){
		fprintf(stderr, "Error: %s could not open.", filename);
		return 1;
	}

	fmtchunksize = 16;
	blocksize = snd->channelnum * (snd->bit_per_sample/8);
	dataspeed = snd->samplingrate * blocksize;
	datasize = snd->datanum * blocksize;
	fswrh = datasize + HEADERSIZE - 8;
	fmtid = 1;

	header_buf[0] = 'R';
	header_buf[1] = 'I';
	header_buf[2] = 'F';
	header_buf[3] = 'F';
	memcpy(header_buf + 4, &fswrh, sizeof(fswrh));
	header_buf[8] = 'W';
	header_buf[9] = 'A';
	header_buf[10] = 'V';
	header_buf[11] = 'E';
	header_buf[12] = 'f';
	header_buf[13] = 'm';
	header_buf[14] = 't';
	header_buf[15] = ' ';
	memcpy(header_buf + 16, &fmtchunksize, sizeof(fmtchunksize));
	memcpy(header_buf + 20, &fmtid, sizeof(fmtid));
	memcpy(header_buf + 22, &(snd->channelnum), sizeof(snd->channelnum));
	memcpy(header_buf + 24, &(snd->samplingrate), sizeof(snd->samplingrate));
	memcpy(header_buf + 28, &dataspeed, sizeof(dataspeed));
	memcpy(header_buf + 32, &blocksize, sizeof(blocksize));
	memcpy(header_buf + 34, &(snd->bit_per_sample), sizeof(snd->bit_per_sample));
	header_buf[36] = 'd';
	header_buf[37] = 'a';
	header_buf[38] = 't';
	header_buf[39] = 'a';
	memcpy(header_buf + 40, &datasize, sizeof(datasize));

	fwrite(header_buf, sizeof(unsigned char), HEADERSIZE, fp);

	if(snd->channelnum==1 && snd->bit_per_sample==8){
		fwrite(snd->monaural8, sizeof(unsigned char), snd->datanum, fp);		//$B%G!<%?ItJ,$rA4$F=q$-9~$`(B
	}else if(snd->channelnum==1 && snd->bit_per_sample==16){
		fwrite(snd->monaural16, sizeof(signed short), snd->datanum, fp);
	}else if(snd->channelnum==2 && snd->bit_per_sample==8){
		for(i=0; i<snd->datanum; i++){
			fwrite(&(snd->stereo8[i].l), sizeof(unsigned char), 1, fp);
			fwrite(&(snd->stereo8[i].r), sizeof(unsigned char), 1, fp);
		}
	}else{
		for(i=0; i<snd->datanum; i++){
			fwrite(&(snd->stereo16[i].l), sizeof(signed short), 1, fp);
			fwrite(&(snd->stereo16[i].r), sizeof(signed short), 1, fp);
		}
	}

	fclose(fp);

	return 0;
}

Sound *Create_Sound(unsigned short channelnum, unsigned long samplingrate, unsigned short bit_per_sample, unsigned long datasize)
{
	Sound *snd;

	if((snd = (Sound *)malloc(sizeof(Sound))) == NULL){
		fprintf(stderr, "Allocation error\n");
		return NULL;
	}

	snd->channelnum = channelnum;
	snd->samplingrate = samplingrate;
	snd->bit_per_sample = bit_per_sample;
	snd->datanum = datasize / (channelnum*(bit_per_sample/8));

	snd->monaural8 = NULL;
	snd->monaural16 = NULL;
	snd->stereo8 = NULL;
	snd->stereo16 = NULL;

	if(channelnum == 1 && bit_per_sample == 8){
		if((snd->monaural8 = (unsigned char *)malloc(datasize)) == NULL){
			fprintf(stderr, "Allocation error\n");
			free(snd);
			return NULL;
		}
	}else if(channelnum == 1 && bit_per_sample == 16){
		if((snd->monaural16 = (signed short *)malloc(sizeof(signed short)*snd->datanum)) == NULL){
			fprintf(stderr, "Allocation error\n");
			free(snd);
			return NULL;
		}
	}else if(channelnum == 2 && bit_per_sample == 8){
		if((snd->stereo8 = (Soundsample8 *)malloc(sizeof(Soundsample8)*snd->datanum)) == NULL){
			fprintf(stderr, "Allocation error\n");
			free(snd);
			return NULL;
		}
	}else if(channelnum == 2 && bit_per_sample == 16){
		if((snd->stereo16 = (Soundsample16 *)malloc(sizeof(Soundsample16)*snd->datanum)) == NULL){
			fprintf(stderr, "Allocation error\n");
			free(snd);
			return NULL;
		}
	}else{
		fprintf(stderr, "Channelnum or Bit/Sample unknown");
		free(snd);
		return NULL;
	}

	return snd;
}

void Free_Sound(Sound *snd)
{
	if(snd->channelnum == 1 && snd->bit_per_sample == 8){
		free(snd->monaural8); 
	}else if(snd->channelnum == 1 && snd->bit_per_sample == 16){
		free(snd->monaural16);
	}else if(snd->channelnum == 2 && snd->bit_per_sample == 8){
		free(snd->stereo8);
	}else{
		free(snd->stereo16);
	}

	free(snd);
}


