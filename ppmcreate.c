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
} ppm_image;

size_t ppm_save(ppm_image *img, FILE *outfile) {
    size_t n = 0;
    uint8_t *data = img->data;

    int x,y;

    n += fprintf(outfile, "P6\n# THIS IS A COMMENT\n%d %d\n%d\n",
                 img->width, img->height, 0xFF);

    for(y = 0; y < img->height; y++) {
        for(x = 0; x < img->width; x++) {
            n += fwrite(data, 1, 3, outfile);
            data += 4;
        }
    }

    return n;
}

int main(int argc, char **argv) {

    char *infilename = NULL;
    char *outfilename = NULL;

    FILE* infile;
    FILE* outfile;

    int c;

    ppm_image img;
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
    outfile = fopen(outfilename, "w");

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

    ppm_save(&img, outfile);

    free(img.data);
    fclose(infile);
    fclose(outfile);

    return 0;
}