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

#ifndef SRC_IMAGE_UTILS_H_
#define SRC_IMAGE_UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <dirent.h>
#include <string.h>
#include <xstatus.h>
#include "platform_conf.h"
#include "terminal_colors.h"

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
 * Allocates needed memory for storing the image in a matrix representation
 * on the image->channels field.
 * @param[in,out] image
 */
void initChannelsMatrix(Image *image) {
	/** Allocate memory for first dimension - color channels */
	image->channels =
		(unsigned char ***) malloc(image->depth * sizeof(unsigned char *));
	if (image->channels == NULL) {
		printf(
			"%sError. Not enough memory for initChannelsMatrix "
			"image->channels.%s\n",
			KRED,
			KNRM);
		exit(XST_FAILURE);
	}

	for (int i = 0; i < image->depth; ++i) {
		/** Initialize every color channel to store all image's rows */
		image->channels[i] =
			(unsigned char **) malloc(image->height * sizeof(unsigned char *));
		if (image->channels[i] == NULL) {
			printf(
				"%sError. Not enough memory for initChannelsMatrix "
				"image->channels[i].%s\n",
				KRED,
				KNRM);
			exit(XST_FAILURE);
		}

		for (int j = 0; j < image->height; j++) {
			/** Initialize every row to store all row's pixels */
			image->channels[i][j] =
				(unsigned char *) malloc(image->width * sizeof(unsigned char));
			if (image->channels[i][j] == NULL) {
				printf(
					"%sError. Not enough memory for initChannelsMatrix "
					"image->channels[i][j].%s\n",
					KRED,
					KNRM);
				exit(XST_FAILURE);
			}
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
				image->channels[k][i][j] =
					image->bmp_buffer
						[k + j * image->depth +
						 i * image->width * image->depth];
			}
		}
	}

	/**
	 * Free image's bmp_buffer to avoid data duplication and eliminate
	 * needed memory
	 */
	free(image->bmp_buffer);
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
	image->bmp_buffer =
		(unsigned char *) malloc(image->bmp_size * sizeof(unsigned char));
	if (image->bmp_buffer == NULL) {
		printf(
			"%sError. Not enough memory for convertToBuffer "
			"image->bmp_buffer.%s\n",
			KRED,
			KNRM);
		exit(XST_FAILURE);
	}

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
}

/**
 * Concatenates two strings, one after the other
 * @param s1: First string
 * @param s2: Second string
 * @returns "s1s2" (e.g. s1 = "example ", s2 = "given", return = "example
 * given")
 */
char *concat(char *s1, char *s2) {
	/** Get strings lengths */
	const size_t len1 = strlen(s1);
	const size_t len2 = strlen(s2);

	/** Allocate memory for the new string, adding 1 for the null-terminator */
	char *result = (char *) malloc((len1 + len2 + 1) * sizeof(char));
	if (result == NULL) {
		printf("%sError. Not enough memory for concat result.%s\n", KRED, KNRM);
		exit(XST_FAILURE);
	}

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

int compare(char **a, char **b) {
	return strcmp(*a, *b);
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
	if (filelist == NULL) {
		printf(
			"%sError. Not enough memory for getFileList filelist.%s\n",
			KRED,
			KNRM);
		exit(XST_FAILURE);
	}
	/** Get the number of files contained in path */
	filelist->length = getFileCount(path);

	/** Allocate necessary memory to store all file paths */
	filelist->list = (char **) malloc(filelist->length * sizeof(char *));
	if (filelist->list == NULL) {
		printf(
			"%sError. Not enough memory for getFileList filelist->list.%s\n",
			KRED,
			KNRM);
		exit(XST_FAILURE);
	}

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

	qsort(
		filelist->list,
		filelist->length,
		sizeof(char *),
		(int (*)(const void *, const void *)) compare);

	return filelist;
}

/**
 * Creates a new image and initializes it
 * @param depth: new image's color profile
 * @param height: new image's height
 * @param width: new image's width
 * @param path: new image's path
 * @returns a new initialized image
 */
Image *createInitializedImage(uint depth, uint height, uint width, char *path) {
	/** Allocate memory for the new image */
	Image *image = (Image *) malloc(sizeof(Image));
	if (image == NULL) {
		printf(
			"%sError. Not enough memory for createInitializedImage image.%s\n",
			KRED,
			KNRM);
		exit(XST_FAILURE);
	}

	/** Initialize new image's dimensions */
	image->depth = depth;
	image->height = height;
	image->width = width;

	/** Copy path to new image */
	image->path = (char *) malloc(strlen(path) * sizeof(char));
	if (image->path == NULL) {
		printf(
			"%sError. Not enough memory for createInitializedImage "
			"image->path.%s\n",
			KRED,
			KNRM);
		exit(XST_FAILURE);
	}

	strcpy(image->path, path);

	/** Allocate memory for new image's channel matrix representation */
	initChannelsMatrix(image);
	return image;
}

#endif
