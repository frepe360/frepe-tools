#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

typedef struct {
    int width;
    int height;
    uint8_t *data;
    size_t size;
} bmp_image;

size_t bmp_save(bmp_image *img, FILE *outfile) {

    unsigned int headers[13];
    int extrabytes;
    int paddedsize;
    int x, y, n;
    int red, green, blue;
    uint8_t* data = img->data;

    // How many bytes of padding to add to each
    // horizontal line - the size of which must
    // be a multiple of 4 bytes.
    extrabytes = 4 - ((img->width * 3) % 4);

    if (extrabytes == 4) {
        extrabytes = 0;
    }

    paddedsize = ((img->width * 3) + extrabytes) * img->height;

    // Headers...
    // Note that the "BM" identifier in bytes 0 and 1 is NOT included in these "header

    headers[0]  = paddedsize + 54;      // bfSize (whole file size)
    headers[1]  = 0;                    // bfReserved (both)
    headers[2]  = 54;                   // bfOffbits
    headers[3]  = 40;                   // biSize
    headers[4]  = img->width;  // biWidth
    headers[5]  = img->height; // biHeight

    // Would have biPlanes and biBitCount in position 6, but they're shorts.
    // It's easier to write them out separately (see below) than pretend
    // they're a single int, especially with endian issues...

    headers[7]  = 0;                    // biCompression
    headers[8]  = paddedsize;           // biSizeImage
    headers[9]  = 0;                    // biXPelsPerMeter
    headers[10] = 0;                    // biYPelsPerMeter
    headers[11] = 0;                    // biClrUsed
    headers[12] = 0;                    // biClrImportant

    //
    // Headers begin...
    // When printing ints and shorts, we write out 1 character at a time to avoid endian issues.
    //

    fprintf(outfile, "BM");

    for (n = 0; n <= 5; n++)
    {
       fprintf(outfile, "%c", headers[n] & 0x000000FF);
       fprintf(outfile, "%c", (headers[n] & 0x0000FF00) >> 8);
       fprintf(outfile, "%c", (headers[n] & 0x00FF0000) >> 16);
       fprintf(outfile, "%c", (headers[n] & (unsigned int) 0xFF000000) >> 24);
    }

    // These next 4 characters are for the biPlanes and biBitCount fields.

    fprintf(outfile, "%c", 1);
    fprintf(outfile, "%c", 0);
    fprintf(outfile, "%c", 24);
    fprintf(outfile, "%c", 0);

    for (n = 7; n <= 12; n++)
    {
       fprintf(outfile, "%c", headers[n] & 0x000000FF);
       fprintf(outfile, "%c", (headers[n] & 0x0000FF00) >> 8);
       fprintf(outfile, "%c", (headers[n] & 0x00FF0000) >> 16);
       fprintf(outfile, "%c", (headers[n] & (unsigned int) 0xFF000000) >> 24);
    }

    //
    // Headers done, now write the data...
    //

    // BMP image format is written from bottom to top...
    for (y = img->height - 1; y >= 0; y--) {
        for (x = 0; x < img->width; x++) {
            red   = data[4 * (y * img->width + x) + 0];
            green = data[4 * (y * img->width + x) + 1];
            blue  = data[4 * (y * img->width + x) + 2];

            // Also, it's written in (b,g,r) format...

            fprintf(outfile, "%c", blue);
            fprintf(outfile, "%c", green);
            fprintf(outfile, "%c", red);
       }

       if (extrabytes)      // See above - BMP lines must be of lengths divisible by 4.
       {
            for (n = 1; n <= extrabytes; n++)
            {
                fprintf(outfile, "%c", 0);
            }
       }
    }


}

int main(int argc, char **argv) {

    char *infilename = NULL;
    char *outfilename = NULL;

    FILE* infile;
    FILE* outfile;

    int c;

    bmp_image img;
    struct stat st;

    opterr = 0;
    while ((c = getopt (argc, argv, "h:w:i:o:")) != -1)
        switch (c) {
            case 'h':
                img.height = atoi(optarg);
                break;
            case 'w':
                img.width = atoi(optarg);
                break;
            case 'i':
                infilename = optarg;
                break;
            case 'o':
                outfilename = optarg;
                break;
            case '?':
                if (optopt == 'c')
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint (optopt))
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf (stderr,
                        "Unknown option character `\\x%x'.\n",
                        optopt);
                return 1;
          default:
                abort ();
          }

    infile = fopen(infilename, "r");
    outfile = fopen(outfilename, "wb");

    if(!infile) {
        printf("%s does not exist, aborting\n", infilename);
        return 0;
        fclose(outfile);
    }

    printf("Infile : %s\n", infilename);
    printf("Outfile: %s\n", outfilename);
    printf("image resolution: %d x %d\n", img.width, img.height);

    fstat(fileno(infile), &st);
    printf("image size: %d\n", (unsigned int)st.st_size);

    img.data = malloc(st.st_size);
    img.size = st.st_size;

    fread(img.data, st.st_size, 1, infile);

    bmp_save(&img, outfile);

    free(img.data);
    fclose(infile);
    fclose(outfile);

    return 0;
}