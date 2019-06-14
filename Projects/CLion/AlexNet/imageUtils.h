// https://gist.github.com/PhirePhly/3080633
// memdjpeg - A super simple example of how to decode a jpeg in memory
// Kenneth Finnegan, 2012
// blog.thelifeofkenneth.com
//
// After installing jpeglib, compile with:
// cc memdjpeg.c -ljpeg -o memdjpeg
//
// Run with:
// ./memdjpeg filename.jpg
//
// Version	   Date		Time		  By
// -------	----------	-----		---------
// 0.01		2012-07-09	11:18		Kenneth Finnegan
//


typedef struct Image_t {
	int height;
	int width;
	int depth;
	int row_stride;
	unsigned char *bmp_buffer;
	unsigned long bmp_size;
	char *filename;
	u_int8_t ***channels;
} Image;


typedef struct filelist_t {
	uint length;
	char **list;
} Filelist;


void freeImageChannels(Image *image) {
	for (int i = 0; i < image->depth; i++) {
		for (int j = 0; j < image->height; j++) {
			free(image->channels[i][j]);
		}
		free(image->channels[i]);
	}
	free(image->channels);
}


void freeImage(Image *image){
	freeImageChannels(image);
	free(image->filename);
	free(image);
}


void readFileAndDecompress(Image *image) {
	syslog(LOG_INFO, "Filename: %s", image->filename);

	//   SSS    EEEEEEE  TTTTTTT  U     U  PPPP
	// SS   SS  E           T     U     U  P   PP
	// S        E           T     U     U  P    PP
	// SS       E           T     U     U  P   PP
	//   SSS    EEEE        T     U     U  PPPP
	//      SS  E           T     U     U  P
	//       S  E           T     U     U  P
	// SS   SS  E           T      U   U   P
	//   SSS    EEEEEEE     T       UUU    P

	// Variables for the source jpg
	struct stat file_info;
	unsigned long jpg_size;
	unsigned char *jpg_buffer;

	// Variables for the decompressor itself
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;


	// Load the jpeg data from a file into a memory buffer for
	// the purpose of this demonstration.
	// Normally, if it's a file, you'd use jpeg_stdio_src, but just
	// imagine that this was instead being downloaded from the Internet
	// or otherwise not coming from disk
	int rc = stat(image->filename, &file_info);
	if (rc) {
		syslog(LOG_ERR, "FAILED to stat source jpg");
		exit(EXIT_FAILURE);
	}
	jpg_size = file_info.st_size;
	jpg_buffer = (unsigned char *) malloc(jpg_size + 100);

	int fd = open(image->filename, O_RDONLY);
	int i = 0;
	while (i < jpg_size) {
		rc = read(fd, jpg_buffer + i, jpg_size - i);
//		syslog(LOG_INFO, "Input: Read %d/%lu bytes", rc, jpg_size - i);
		i += rc;
	}
	close(fd);

	//   SSS    TTTTTTT     A     RRRR     TTTTTTT
	// SS   SS     T       A A    R   RR      T
	// S           T      A   A   R    RR     T
	// SS          T     A     A  R   RR      T
	//   SSS       T     AAAAAAA  RRRR        T
	//      SS     T     A     A  R RR        T
	//       S     T     A     A  R   R       T
	// SS   SS     T     A     A  R    R      T
	//   SSS       T     A     A  R     R     T

//	syslog(LOG_INFO, "Proc: Create Decompress struct");
	// Allocate a new decompress struct, with the default error handler.
	// The default error handler will exit() on pretty much any issue,
	// so it's likely you'll want to replace it or supplement it with
	// your own.
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);


//	syslog(LOG_INFO, "Proc: Set memory buffer as source");
	// Configure this decompressor to read its data from a memory
	// buffer starting at unsigned char *jpg_buffer, which is jpg_size
	// long, and which must contain a complete jpg already.
	//
	// If you need something fancier than this, you must write your
	// own data source manager, which shouldn't be too hard if you know
	// what it is you need it to do. See jpeg-8d/jdatasrc.c for the
	// implementation of the standard jpeg_mem_src and jpeg_stdio_src
	// managers as examples to work from.
	jpeg_mem_src(&cinfo, jpg_buffer, jpg_size);


//	syslog(LOG_INFO, "Proc: Read the JPEG header");
	// Have the decompressor scan the jpeg header. This won't populate
	// the cinfo struct output fields, but will indicate if the
	// jpeg is valid.
	rc = jpeg_read_header(&cinfo, TRUE);

	if (rc != 1) {
		syslog(LOG_ERR, "File does not seem to be a normal JPEG");
		exit(EXIT_FAILURE);
	}

//	syslog(LOG_INFO, "Proc: Initiate JPEG decompression");
	// By calling jpeg_start_decompress, you populate cinfo
	// and can then allocate your output bitmap buffers for
	// each scanline.
	jpeg_start_decompress(&cinfo);

	image->width = cinfo.output_width;
	image->height = cinfo.output_height;
	image->depth = cinfo.output_components;

//	syslog(LOG_INFO, "Proc: Image is %d by %d with %d components", image->width, image->height, image->depth);

	image->bmp_size = image->width * image->height * image->depth;
	image->bmp_buffer = (unsigned char *) malloc(image->bmp_size);

	// The row_stride is the total number of bytes it takes to store an
	// entire scanline (row).
	image->row_stride = image->width * image->depth;


//	syslog(LOG_INFO, "Proc: Start reading scanlines");

	// Now that you have the decompressor entirely configured, it's time
	// to read out all of the scanlines of the jpeg.
	//
	// By default, scanlines will come out in RGBRGBRGB...  order,
	// but this can be changed by setting cinfo.out_color_space
	//
	// jpeg_read_scanlines takes an array of buffers, one for each scanline.
	// Even if you give it a complete set of buffers for the whole image,
	// it will only ever decompress a few lines at a time. For best
	// performance, you should pass it an array with cinfo.rec_outbuf_height
	// scanline buffers. rec_outbuf_height is typically 1, 2, or 4, and
	// at the default high quality decompression setting is always 1.
	while (cinfo.output_scanline < cinfo.output_height) {
		unsigned char *buffer_array[1];
		buffer_array[0] = image->bmp_buffer + (cinfo.output_scanline) * image->row_stride;
		jpeg_read_scanlines(&cinfo, buffer_array, 1);
	}
//	syslog(LOG_INFO, "Proc: Done reading scanlines");


	// Once done reading *all* scanlines, release all internal buffers,
	// etc by calling jpeg_finish_decompress. This lets you go back and
	// reuse the same cinfo object with the same settings, if you
	// want to decompress several jpegs in a row.
	//
	// If you didn't read all the scanlines, but want to stop early,
	// you instead need to call jpeg_abort_decompress(&cinfo)
	jpeg_finish_decompress(&cinfo);

	// At this point, optionally go back and either load a new jpg into
	// the jpg_buffer, or define a new jpeg_mem_src, and then start
	// another decompress operation.

	// Once you're really really done, destroy the object to free everything
	jpeg_destroy_decompress(&cinfo);
	// And free the input buffer
	free(jpg_buffer);

	// DDDD       OOO    N     N  EEEEEEE
	// D  DDD    O   O   NN    N  E
	// D    DD  O     O  N N   N  E
	// D     D  O     O  N N   N  E
	// D     D  O     O  N  N  N  EEEE
	// D     D  O     O  N   N N  E
	// D    DD  O     O  N   N N  E
	// D  DDD    O   O   N    NN  E
	// DDDD       OOO    N     N  EEEEEEE
	syslog(LOG_INFO, "Done decompressing.");
}


void initChannelsMatrix(Image *image) {
	image->channels = (u_int8_t ***) malloc(image->depth * sizeof(u_int8_t *));
	for (int i = 0; i < image->depth; ++i) {
		image->channels[i] = (u_int8_t **) malloc(image->height * sizeof(u_int8_t *));
		for (int j = 0; j < image->height; j++) {
			image->channels[i][j] = (u_int8_t *) malloc(image->width * sizeof(u_int8_t));
		}
	}
}


void convertToMatrix(Image *image) {
	initChannelsMatrix(image);

	for (int i = 0; i < image->height; i++) {
		for (int j = 0; j < image->width; j++) {
			for (int k = 0; k < image->depth; k++) {
				image->channels[k][i][j] = image->bmp_buffer[k + j * image->depth + i * image->width * image->depth];
			}
		}
	}

	free(image->bmp_buffer);
	syslog(LOG_INFO, "Done converting raw image to uint8 matrix.");
}


void convertToBuffer(Image *image) {
	image->bmp_size = image->depth * image->width * image->height;
	image->bmp_buffer = (unsigned char *) malloc(image->bmp_size * sizeof(unsigned char));

	uint index = 0;
	for (int y = 0; y < image->height; y++) {
		for (int x = 0; x < image->width; x++) {
			for (int d = 0; d < image->depth; d++) {
				image->bmp_buffer[index] = image->channels[d][y][x];
				index++;
			}
		}
	}
	syslog(LOG_INFO, "Done converting image to buffer.");
}


void writeToFile(Image *image) {
	convertToBuffer(image);
	// Write the decompressed bitmap out to a ppm file, just to make sure it worked.
	int fd = open("../output.ppm", O_CREAT | O_WRONLY, 0666);
	char buf[1024];

	int rc = sprintf(buf, "P6 %d %d 255\n", image->width, image->height);
	write(fd, buf, rc); // Write the PPM image header before data
	write(fd, image->bmp_buffer, image->bmp_size); // Write out all RGB pixel data

	close(fd);
	syslog(LOG_INFO, "Done writing image to ppm file.");
}


char *concat(char *s1, char *s2) {
	const size_t len1 = strlen(s1);
	const size_t len2 = strlen(s2);
	char *result = malloc(len1 + len2 + 1); // +1 for the null-terminator
	memcpy(result, s1, len1);
	memcpy(result + len1, s2, len2 + 1); // +1 to copy the null-terminator
	return result;
}


uint getFileCount(char *path) {
	struct dirent *de;  // Pointer for directory entry

	// opendir() returns a pointer of DIR type.
	DIR *dr = opendir(path);

	// opendir returns NULL if couldn't open directory
	if (dr == NULL) {
		fprintf(stderr, "Could not open current directory");
		return -1;
	}

	// Refer http://pubs.opengroup.org/onlinepubs/7990989775/xsh/readdir.html
	// for readdir()
	uint count = 0;
	while ((de = readdir(dr)) != NULL) {
		if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, "..")) continue;

		char *innerPath = concat(path, de->d_name);

		DIR *drInner = opendir(innerPath);
		struct dirent *deInner;
		while ((deInner = readdir(drInner)) != NULL) {
			if (!strcmp(deInner->d_name, ".") || !strcmp(deInner->d_name, "..")) {
				continue;
			}
			count++;
		}
		closedir(drInner);
	}
	closedir(dr);
	free(de);
	return count;
}


Filelist *getFileList(char *path) {
	Filelist *filelist = (Filelist *) malloc(sizeof(Filelist));
	filelist->length = getFileCount(path);
	filelist->list = (char **) malloc(filelist->length * sizeof(char *));
	uint index = 0;
	DIR *dr = opendir(path);
	struct dirent *de;
	while ((de = readdir(dr)) != NULL) {
		if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, "..")) continue;

		char *innerPath = concat(path, de->d_name);

		DIR *drInner = opendir(innerPath);
		struct dirent *deInner;
		while ((deInner = readdir(drInner)) != NULL) {
			if (!strcmp(deInner->d_name, ".") || !strcmp(deInner->d_name, "..")) continue;
			char *tmp = concat(innerPath, "/");
			filelist->list[index] = concat(tmp, deInner->d_name);
			index++;
			free(tmp);
		}
		closedir(drInner);
		free(innerPath);
		free(deInner);
	}
	closedir(dr);
	free(de);

	return filelist;
}


Image *upScale(Image *image, uint dim) {
	uint x = dim / image->width + (dim % image->width ? 1 : 0);
	uint y = dim / image->height + (dim % image->height ? 1 : 0);

	uint upscaleRatio = x > y ? x : y;

	Image *newImage = (Image *) malloc(sizeof(Image));

	newImage->width = image->width * upscaleRatio;
	newImage->height = image->height * upscaleRatio;
	newImage->depth = image->depth;

	newImage->filename = (char *) malloc(strlen(image->filename) * sizeof(char));
	strcpy(newImage->filename, image->filename);

	initChannelsMatrix(newImage);

	for (int k = 0; k < image->depth; k++) {
		for (int i = 0; i < image->height; i++) {
			for (int j = 0; j < image->width; j++) {

				for (int l = 0; l < upscaleRatio; l++) {
					for (int m = 0; m < upscaleRatio; m++) {
						newImage->channels[k][i * upscaleRatio + l][j * upscaleRatio + m] = image->channels[k][i][j];
					}
				}

			}
		}
	}

	syslog(LOG_INFO, "Done upscaling image with %u ratio. New image width: %u, height: %u", upscaleRatio, newImage->width, newImage->height);
	return newImage;
}


Image *downScale(Image *image, uint compressionRatio) {
	Image *newImage = (Image *) malloc(sizeof(Image));

	newImage->width = image->width / compressionRatio + (image->width % compressionRatio ? 1 : 0);
	newImage->height = image->height / compressionRatio + (image->height % compressionRatio ? 1 : 0);
	newImage->depth = image->depth;

	newImage->filename = (char *) malloc(strlen(image->filename) * sizeof(char));
	strcpy(newImage->filename, image->filename);

	initChannelsMatrix(newImage);

	for (int k = 0; k < image->depth; k++) {
		for (int i = 0; i < newImage->height; i++) {
			for (int j = 0; j < newImage->width; j++) {
				uint pixel = 0;
				uint count = 0;

				for (int l = 0; l < compressionRatio && (i * compressionRatio + l) < image->height; l++) {
					for (int m = 0; m < compressionRatio && (j * compressionRatio + m) < image->width; m++) {
						pixel += image->channels[k][i * compressionRatio + l][j * compressionRatio + m];
						count++;
					}
				}

				newImage->channels[k][i][j] = pixel / count;
			}
		}
	}

	syslog(LOG_INFO, "Done compressing image with %u ratio.", compressionRatio);
	return newImage;
}


Image *imageResize(Image *image, uint dim) {
	uint x = image->width / dim;
	uint y = image->height / dim;

	uint compressionRatio;
	if (x < y) {
		compressionRatio = x;
	} else {
		compressionRatio = y;
	}

	if (compressionRatio == 1) {
		syslog(LOG_INFO, "No resizing needed.");
		return image;
	}

	if (compressionRatio == 0) return upScale(image, dim);
	return downScale(image, compressionRatio);
}


Image *imageCenterCrop(Image *image, uint dim) {
	Image *newImage = (Image *) malloc(sizeof(Image));
	newImage->width = dim;
	newImage->height = dim;
	newImage->depth = image->depth;

	newImage->filename = (char *) malloc(strlen(image->filename) * sizeof(char));
	strcpy(newImage->filename, image->filename);

	initChannelsMatrix(newImage);

	int startY = (image->height - dim) / 2;
	int startX = (image->width - dim) / 2;

	for (int k = 0; k < image->depth; k++) {
		for (int i = 0; i < dim; i++) {
			for (int j = 0; j < dim; j++) {
				newImage->channels[k][i][j] = image->channels[k][startY + i][startX + j];
			}
		}
	}

	syslog(LOG_INFO, "Done cropping.");
	return newImage;
}