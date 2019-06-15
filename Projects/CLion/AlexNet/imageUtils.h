/**
 * Library that contains various tools for jpeg image handling.
 * Based on https://gist.github.com/PhirePhly/3080633
 *
 * Compile using "jpeg" linker library.
 * An installation may be needed by using packets such as
 * jpeglib (Arch based) or libjpeg (Debian based).
 *
 * Compile with: cc project_name.c -ljpeg -o project_name
 */


/** Contains all necessary data for an image to be stored in memory. */
typedef struct Image_t {
	/** Image's height */
	int height;
	/** Image's width */
	int width;
	/** Image's color profile (RGB -> 3, BW -> 1, CMYK -> 4, etc.) */
	int depth;
	/**
	 * Holds RGB data after jpeg decompression, one color in each cell,
	 * flattened row by row
	 */
	unsigned char *bmp_buffer;
	/** Size of bmp buffer in cells */
	unsigned long bmp_size;
	/** Image's path */
	char *path;
	/**
	 * A 3-dimensional representation of an image.
	 * First dimension: Color channel
	 * Second dimension: Image's rows in pixels
	 * Third dimension: Image's columns in pixels
	 *
	 * Valid pixel values: unsigned integers in range (0, 255).
	 */
	u_int8_t ***channels;
} Image;


/** Structure to contain file paths */
typedef struct filelist_t {
	/** Length of the list (number of paths stored) */
	uint length;
	/** List of strings to store filepaths */
	char **list;
} Filelist;


/**
 * Frees image's channels matrix
 * @param[in] image
 */
void freeImageChannels(Image *image) {
	for (int i = 0; i < image->depth; i++) {
		for (int j = 0; j < image->height; j++) {
			/** Free every row */
			free(image->channels[i][j]);
		}
		/** Free every color channel */
		free(image->channels[i]);
	}
	/** Free channels array */
	free(image->channels);
}


/**
 * Frees image's channels, path and its given structure
 * @param[in] image
 */
void freeImage(Image *image) {
	freeImageChannels(image);
	free(image->path);
	free(image);
}


/**
 * Reads a jpeg file and decompresses it to given image->bpm_buffer.
 * @param[in,out] image
 */
void readFileAndDecompress(Image *image) {
	syslog(LOG_INFO, "Filename: %s", image->path);

	/** Variables for the source jpg */
	struct stat file_info;
	unsigned long jpg_size;
	unsigned char *jpg_buffer;

	/** Variables for the decompressor itself */
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;


	/**
	 * Load the jpeg data from a file into a memory buffer for the purpose of
	 * this demonstration. Normally, if it's a file, you'd use jpeg_stdio_src,
	 * but just imagine that this was instead being downloaded from the
	 * Internet or otherwise not coming from disk
	 */
	int rc = stat(image->path, &file_info);
	if (rc) {
		syslog(LOG_ERR, "FAILED to stat source jpg");
		exit(EXIT_FAILURE);
	}
	jpg_size = file_info.st_size;
	jpg_buffer = (unsigned char *) malloc(jpg_size + 100);

	int fd = open(image->path, O_RDONLY);
	int i = 0;
	while (i < jpg_size) {
		rc = read(fd, jpg_buffer + i, jpg_size - i);
		// syslog(LOG_INFO, "Input: Read %d/%lu bytes", rc, jpg_size - i);
		i += rc;
	}
	close(fd);


	// syslog(LOG_INFO, "Proc: Create Decompress struct");
	/**
	 * Allocate a new decompress struct, with the default error handler.
	 * The default error handler will exit() on pretty much any issue,
	 * so it's likely you'll want to replace it or supplement it with your own.
	 */
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);


	// syslog(LOG_INFO, "Proc: Set memory buffer as source");
	/**
	 * Configure this decompressor to read its data from a memory buffer
	 * starting at unsigned char *jpg_buffer, which is jpg_size long, and which
	 * must contain a complete jpg already. If you need something fancier than
	 * this, you must write your own data source manager, which shouldn't be
	 * too hard if you know what it is you need it to do.
	 *
	 * See jpeg-8d/jdatasrc.c for the implementation of the standard
	 * jpeg_mem_src and jpeg_stdio_src managers as examples to work from.
	 */
	jpeg_mem_src(&cinfo, jpg_buffer, jpg_size);


	// syslog(LOG_INFO, "Proc: Read the JPEG header");
	/**
	 * Have the decompressor scan the jpeg header. This won't populate the
	 * cinfo struct output fields, but will indicate if the jpeg is valid.
	 */
	rc = jpeg_read_header(&cinfo, TRUE);

	if (rc != 1) {
		syslog(LOG_ERR, "File does not seem to be a normal JPEG");
		exit(EXIT_FAILURE);
	}

	// syslog(LOG_INFO, "Proc: Initiate JPEG decompression");
	/** By calling jpeg_start_decompress, you populate cinfo and can then
	 * allocate your output bitmap buffers for each scanline.
	 */
	jpeg_start_decompress(&cinfo);

	image->width = cinfo.output_width;
	image->height = cinfo.output_height;
	image->depth = cinfo.output_components;

	//	syslog(LOG_INFO, "Proc: Image is %d by %d with %d components",
	//	       image->width, image->height, image->depth);

	image->bmp_size = image->width * image->height * image->depth;
	image->bmp_buffer = (unsigned char *) malloc(image->bmp_size);

	/**
	 * The row_stride is the total number of bytes it takes to store an
	 * entire scanline (row).
	 */
	uint row_stride = image->width * image->depth;


	// syslog(LOG_INFO, "Proc: Start reading scanlines");

	/**
	 * 	Now that you have the decompressor entirely configured, it's time to
	 * 	read out all of the scanlines of the jpeg. By default, scanlines will
	 * 	come out in RGBRGBRGB...  order, but this can be changed by setting
	 * 	cinfo.out_color_space.
	 *
	 * 	jpeg_read_scanlines takes an array of buffers, one for each scanline.
	 * 	Even if you give it a complete set of buffers for the whole image, it
	 * 	will only ever decompress a few lines at a time.
	 *
	 * 	For best performance, you should pass it an array with
	 * 	cinfo.rec_outbuf_height scanline buffers. rec_outbuf_height is
	 * 	typically 1, 2, or 4, and at the default high quality decompression
	 * 	setting is always 1.
	 */
	while (cinfo.output_scanline < cinfo.output_height) {
		unsigned char *buffer_array[1];
		buffer_array[0] = image->bmp_buffer + (cinfo.output_scanline) * row_stride;
		jpeg_read_scanlines(&cinfo, buffer_array, 1);
	}
	// syslog(LOG_INFO, "Proc: Done reading scanlines");


	/**
	 * Once done reading *all* scanlines, release all internal buffers, etc by
	 * calling jpeg_finish_decompress. This lets you go back and reuse the same
	 * cinfo object with the same settings, if you want to decompress several
	 * jpegs in a row. If you didn't read all the scanlines, but want to stop
	 * early, you instead need to call jpeg_abort_decompress(&cinfo)
	 */
	jpeg_finish_decompress(&cinfo);

	/**
	 * At this point, optionally go back and either load a new jpg into the
	 * jpg_buffer, or define a new jpeg_mem_src, and then start another
	 * decompress operation. Once you're really really done, destroy the object
	 * to free everything
	 */
	jpeg_destroy_decompress(&cinfo);

	/** And free the input buffer */
	free(jpg_buffer);

	syslog(LOG_INFO, "Done decompressing.");
}


/**
 * Allocates needed memory for storing the image in a matrix representation
 * on the image->channels field.
 * @param[in,out] image
 */
void initChannelsMatrix(Image *image) {
	/** Allocate memory for first dimension - color channels */
	image->channels = (u_int8_t ***) malloc(image->depth * sizeof(u_int8_t *));
	for (int i = 0; i < image->depth; ++i) {
		/** Initialize every color channel to store all image's rows */
		image->channels[i] = (u_int8_t **) malloc(image->height * sizeof(u_int8_t *));
		for (int j = 0; j < image->height; j++) {
			/** Initialize every row to store all row's pixels */
			image->channels[i][j] = (u_int8_t *) malloc(image->width * sizeof(u_int8_t));
		}
	}
}


/**
 * Converts the bmp_buffer to a channels matrix representation and stores it
 * into channels field using the given image.
 * @param[in,out] image
 */
void convertToMatrix(Image *image) {
	/** Allocate needed memory */
	initChannelsMatrix(image);

	/** For every row */
	for (int i = 0; i < image->height; i++) {
		/** For every pixel in a row */
		for (int j = 0; j < image->width; j++) {
			/** For every color channel */
			for (int k = 0; k < image->depth; k++) {
				image->channels[k][i][j] = image->bmp_buffer[k + j * image->depth + i * image->width * image->depth];
			}
		}
	}

	/**
	 * Free image's bmp_buffer to avoid data duplication and eliminate
	 * needed memory
	 */
	free(image->bmp_buffer);
	syslog(LOG_INFO, "Done converting raw image to uint8 matrix.");
}


/**
 * Converts the channels matrix representation to a bmp_buffer one and stores it
 * into bmp_buffer field using the given image.
 * @param[in,out] image
 */
void convertToBuffer(Image *image) {
	/** Calculate bmp_buffer size (#total pixels * #color channels) */
	image->bmp_size = image->depth * image->width * image->height;

	/** Allocate needed memory */
	image->bmp_buffer = (unsigned char *) malloc(image->bmp_size * sizeof(unsigned char));

	uint index = 0;
	/** For every row */
	for (int y = 0; y < image->height; y++) {
		/** For every pixel in a row */
		for (int x = 0; x < image->width; x++) {
			/** For every color in a pixel */
			for (int d = 0; d < image->depth; d++) {
				image->bmp_buffer[index] = image->channels[d][y][x];
				index++;
			}
		}
	}
	syslog(LOG_INFO, "Done converting image to buffer.");
}


/**
 * Writes the given image into a file named "output.ppm" using bmp_buffer field
 * @param[in] image
 */
void writeToFile(Image *image) {
	/** Convert given image to bmp_buffer representation */
	convertToBuffer(image);

	/** Write the decompressed bitmap out to a ppm file */
	int fd = open("../output.ppm", O_CREAT | O_WRONLY, 0666);
	char buf[1024];
	int rc = sprintf(buf, "P6 %d %d 255\n", image->width, image->height);

	/** Write the PPM image header before data */
	write(fd, buf, rc);

	/** Write out all RGB pixel data */
	write(fd, image->bmp_buffer, image->bmp_size);

	/**
	 * Close file to make it available for other programs to read while
	 * program is still running
	 */
	close(fd);
	syslog(LOG_INFO, "Done writing image to ppm file.");
}


/**
 * Concatenates two strings, one after the other
 * @param s1: First string
 * @param s2: Second string
 * @returns "s1s2" (e.g. s1 = "example ", s2 = "given", return = "example given")
 */
char *concat(char *s1, char *s2) {
	/** Get strings lengths */
	const size_t len1 = strlen(s1);
	const size_t len2 = strlen(s2);

	/** Allocate memory for the new string, adding 1 for the null-terminator */
	char *result = malloc(len1 + len2 + 1);

	/** Copy s1 to result beginning from its first byte */
	memcpy(result, s1, len1);

	/**
	 * Copy s2 to result beginning from len1 byte and adding one byte for the
	 * null-terminator
	 */
	memcpy(result + len1, s2, len2 + 1);
	return result;
}


/**
 * Checks if given directory entry is current folder or parent folder
 * @param de
 * @returns True if directory is current or parent folder
 */
boolean isCurrentOrParentFolder(struct dirent *de) {
	/**
	 * Current directory name is "."
	 * Parent directory name is ".."
	 */
	return !strcmp(de->d_name, ".") || !strcmp(de->d_name, "..");
}


/**
 * Counts the files existing in the given path's directories
 * @param[in] path
 * @returns number of files found
 */
uint getFileCount(char *path) {
	/** opendir() returns a pointer of DIR type. */
	DIR *dr = opendir(path);

	/** opendir returns NULL if couldn't open directory */
	if (dr == NULL) {
		fprintf(stderr, "Could not open current directory");
		return -1;
	}

	/** Initialize counter */
	uint count = 0;

	/** Pointer for main directory entry */
	struct dirent *de;

	/** Read every folder contained in given path */
	while ((de = readdir(dr)) != NULL) {
		/** Ignore linux's current and parent folders */
		if (isCurrentOrParentFolder(de)) continue;

		/** Create path for the found inner folder */
		char *innerPath = concat(path, de->d_name);

		/** Open inner folder */
		DIR *drInner = opendir(innerPath);

		/** Pointer for inner directory entry */
		struct dirent *deInner;

		/** Read every file in directory */
		while ((deInner = readdir(drInner)) != NULL) {
			/** Ignore linux's current and parent folders */
			if (isCurrentOrParentFolder(deInner)) continue;

			/** Count up */
			count++;
		}

		/** Close inner directory, not useful any more */
		closedir(drInner);

		/** Free useless inner directory entry */
		free(deInner);
	}

	/** Close main directory */
	closedir(dr);

	/** Free useless main directory entry */
	free(de);

	return count;
}


/**
 * Reads a path and creates a Filelist which containes all files contained in
 * the given path's children directories
 * @param[in] path
 * @returns Filelist *
 */
Filelist *getFileList(char *path) {
	/** Allocate necessary memory */
	Filelist *filelist = (Filelist *) malloc(sizeof(Filelist));

	/** Get the number of files contained in path */
	filelist->length = getFileCount(path);

	/** Allocate necessary memory to store all file paths */
	filelist->list = (char **) malloc(filelist->length * sizeof(char *));

	/** Initialize filelist->list index */
	uint index = 0;

	/** Open path's directory */
	DIR *dr = opendir(path);

	/** Pointer for main directory entry */
	struct dirent *de;

	/** Read every folder contained in given path */
	while ((de = readdir(dr)) != NULL) {
		/** Ignore linux's current and parent folders */
		if (isCurrentOrParentFolder(de)) continue;

		/** Create path for the found inner folder */
		char *innerPath = concat(path, de->d_name);

		/** Open inner folder */
		DIR *drInner = opendir(innerPath);

		/** Pointer for inner directory entry */
		struct dirent *deInner;

		/** Read every file in directory */
		while ((deInner = readdir(drInner)) != NULL) {
			/** Ignore linux's current and parent folders */
			if (isCurrentOrParentFolder(deInner)) continue;

			/** Append trailing slash to inner path */
			char *tmp = concat(innerPath, "/");

			/** Append found filename to inner path and store it to filelist */
			filelist->list[index] = concat(tmp, deInner->d_name);

			/** Increase index to point to the next cell */
			index++;

			/** Free useless temporary string */
			free(tmp);
		}

		/** Close inner directory */
		closedir(drInner);

		/** Free useless innerPath string (it is not assigned anywhere */
		free(innerPath);

		/** Free useless deInner */
		free(deInner);
	}

	/** Close main directory */
	closedir(dr);

	/** Free main directory entry */
	free(de);

	return filelist;
}


/**
 * Upscales an image by copying each given image's pixel to squares of pixels
 * preserving same ratio. Every input image's pixel gets divided by
 * upscaleRatio x upscaleRatio pixels of the same value as the initial pixel.
 * Applicable for every color profile.
 * @param[in] image: the image to upscale
 * @param[in] dim: the target minimum dimensions
 * @returns an image upscaled to at least dim x dim dimensions
 */
Image *upScale(Image *image, uint dim) {
	/**
	 * Calculate the number of times each dimension (height, width) has to be
	 * multiplied to get at least dim size
	 */
	uint x = dim / image->width + (dim % image->width ? 1 : 0);
	uint y = dim / image->height + (dim % image->height ? 1 : 0);

	/** Select the biggest multiplier */
	uint upscaleRatio = x > y ? x : y;

	/** Allocate memory for the new upscaled image */
	Image *newImage = (Image *) malloc(sizeof(Image));

	/** Initialize newImage's dimensions */
	newImage->width = image->width * upscaleRatio;
	newImage->height = image->height * upscaleRatio;
	newImage->depth = image->depth;

	/** Copy image's path to newImage for future usage */
	newImage->path = (char *) malloc(strlen(image->path) * sizeof(char));
	strcpy(newImage->path, image->path);

	/** Allocate memory for newImage's channel matrix representation */
	initChannelsMatrix(newImage);

	/** For every color channel */
	for (int k = 0; k < image->depth; k++) {
		/** For every row */
		for (int i = 0; i < image->height; i++) {
			/** For every pixel in a row */
			for (int j = 0; j < image->width; j++) {
				/** Copy upscaleRatio times the pixel in height */
				for (int l = 0; l < upscaleRatio; l++) {
					/** Copy upscaleRatio times the pixel in width */
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


/**
 * Downscales an image by creating a pixel from averaging squares of pixels
 * preserving the same ratio. Every input image's dimension gets divided by the
 * target dim and then the minimum of the two gets selected as compressionRatio.
 * The compressionRatio x compressionRatio pixel squares are then averaged to
 * create a new pixel.
 * Applicable for every color profile.
 * @param image: the image to downscale
 * @param dim: the target dimension
 * @return an image downscaled to at minimum dim x dim dimensions
 */
Image *downScale(Image *image, uint dim) {
	/**
	 * Calculate the number of times each dimension (height, width) has to be
	 * compressed to get at minimum dim size
	 */
	uint x = image->width / dim;
	uint y = image->height / dim;

	/** Select the smallest divisor */
	uint compressionRatio = x < y ? x : y;

	/**
	 * If compressionRatio = 1 then no compression is possible so return
	 * original image
	 */
	if (compressionRatio == 1) return image;

	/** Allocate memory for the new downscaled image */
	Image *newImage = (Image *) malloc(sizeof(Image));

	/** Initialize newImage's dimensions */
	newImage->width = image->width / compressionRatio + (image->width % compressionRatio ? 1 : 0);
	newImage->height = image->height / compressionRatio + (image->height % compressionRatio ? 1 : 0);
	newImage->depth = image->depth;

	/** Copy image's path to newImage for future usage */
	newImage->path = (char *) malloc(strlen(image->path) * sizeof(char));
	strcpy(newImage->path, image->path);

	/** Allocate memory for newImage's channel matrix representation */
	initChannelsMatrix(newImage);

	/** For every color channel */
	for (int k = 0; k < image->depth; k++) {
		/** For every row */
		for (int i = 0; i < newImage->height; i++) {
			/** For every pixel in a row */
			for (int j = 0; j < newImage->width; j++) {
				/**
				 * Initialize pixel to store the square of pixels' sum to calculate its average
				 */
				uint pixel = 0;

				/**
				 * Initialize counter to count the number of pixels summed so
				 * that it will then be used to divide the pixel sum to get
				 * its average
				 */
				uint count = 0;

				/**
				 * For compressionRatio pixels or the number of pixels left on
				 * the height dimension
				 */
				for (int l = 0; l < compressionRatio && (i * compressionRatio + l) < image->height; l++) {
					/**
					 * For compressionRatio pixels or the number of pixels left on
					 * the width dimension
					 */
					for (int m = 0; m < compressionRatio && (j * compressionRatio + m) < image->width; m++) {
						/** Sum up all square's pixels */
						pixel += image->channels[k][i * compressionRatio + l][j * compressionRatio + m];

						/** Count up one pixel at a time */
						count++;
					}
				}

				/**
				 * Calculate the pixels' average and store it to the newImage's
				 * corresponding pixel
				 */
				newImage->channels[k][i][j] = pixel / count;
			}
		}
	}

	syslog(LOG_INFO, "Done compressing image with %u ratio.", compressionRatio);
	return newImage;
}


/**
 * Resizes an image to closest to dim x dim dimensions by upscaling or
 * downscaling and preserving initial ratio
 * @param[in] image: the image to resize
 * @param[in] dim: the target dimensions
 * @return resized image
 */
Image *imageResize(Image *image, uint dim) {
	if (image->width < dim || image->height < dim) {
		/** If image is smaller in at least one dimension than the target then upscale */
		return upScale(image, dim);
	} else if (image->width > dim && image->height > dim) {
		/** If image is bigger in at both dimensions than the target then downscale */
		return downScale(image, dim);
	}
	/** If at least one dimension is same as target, no resizing is needed */
	return image;
}


/**
 * Crops an image to given dim x dim dimensions from the center
 * @param[in] image: the image to get cropped
 * @param[in] dim: the target dimensions
 * @return the cropped dim x dim image
 */
Image *imageCenterCrop(Image *image, uint dim) {
	/** Allocate memory for the new cropped image */
	Image *newImage = (Image *) malloc(sizeof(Image));

	/** Initialize newImage's dimensions */
	newImage->width = dim;
	newImage->height = dim;
	newImage->depth = image->depth;

	/** Copy image's path to newImage for future usage */
	newImage->path = (char *) malloc(strlen(image->path) * sizeof(char));
	strcpy(newImage->path, image->path);

	/** Allocate memory for newImage's channel matrix representation */
	initChannelsMatrix(newImage);

	/** Initialize starting points for each dimension */
	int startY = (image->height - dim) / 2;
	int startX = (image->width - dim) / 2;

	/** For every color channel */
	for (int k = 0; k < image->depth; k++) {
		/** For dim rows */
		for (int i = 0; i < dim; i++) {
			/** For dim pixels in a row */
			for (int j = 0; j < dim; j++) {
				/** Copy original image's pixels to newImage */
				newImage->channels[k][i][j] = image->channels[k][startY + i][startX + j];
			}
		}
	}

	syslog(LOG_INFO, "Done cropping.");
	return newImage;
}